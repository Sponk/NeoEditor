//========================================================================
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <DeferredRenderer.h>
#include <Window/Window.h>
#include <string>

#ifndef SHADER_PATH
#define SHADER_PATH "./"
#endif

namespace Neo
{

static const char* defaultShaderFiles[DEFERRED_SHADERS] = {"DeferredDefault.vert", "DeferredGBuf.frag", "DeferredFinal.frag"};

DeferredRenderer::DeferredRenderer()
{
	// Let's hope we have an GL context
	for(int i = 0; i < DEFERRED_SHADERS; i++)
		m_shaders[i] = NULL;

    m_framebufferID = 0;
    m_gbufferTexID = 0;
    m_depthTexID = 0;

    m_quadTexCoordVBO = 0;
    m_quadVAO = 0;
    m_quadVBO = 0;

    m_lightUpdateThread = NULL;
    m_lightUpdateSemaphore = NULL;
    m_currentScene = NULL;
    m_numVisibleLights = 0;

    m_texCoords[0] = MVector2(0, 1);
    m_texCoords[1] = MVector2(0, 0);
    m_texCoords[3] = MVector2(1, 0);
    m_texCoords[2] = MVector2(1, 1);

	init();
}

DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::drawMesh(Mesh* mesh, OCamera* camera)
{
	int num = mesh->getSubMeshsNumber();
	SubMesh* subMeshes = mesh->getSubMeshs();

    for(int i = 0; i < num; i++)
	{
        drawSubMesh(&subMeshes[i], camera);
	}
}

void DeferredRenderer::drawSubMesh(SubMesh* mesh, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	MRenderingContext* render = engine->getRenderingContext();

    initVBO(mesh);

    MVector3* verts = mesh->getVertices();
	int numVerts = mesh->getVerticesSize();

    for(int i = 0; i < mesh->getDisplaysNumber(); i++)
    {
        drawDisplay(mesh, mesh->getDisplay(i), camera);
    }
}

void DeferredRenderer::drawDisplay(SubMesh* mesh, MaterialDisplay* display, OCamera* camera)
{
    NeoEngine* engine = NeoEngine::getInstance();
    MRenderingContext* render = engine->getRenderingContext();

    // Retrieve VBO information
    unsigned int * vboId1 = mesh->getVBOid1();
    unsigned int * vboId2 = mesh->getVBOid2();
    unsigned int * vao = display->getVAO();

    // Save data into variables
    MVector2* texcoords = mesh->getTexCoords();
    M_TYPES indicesType = mesh->getIndicesType();
    void* indices = mesh->getIndices();

    Material* material = display->getMaterial();
    int texturePasses = material->getTexturesPassNumber();    

    int fx = 0;

    if(*vao == 0)
    {
        fx = m_fx[0];
        render->bindFX(fx);

        render->createVAO(vao);
        render->bindVAO(*vao);

        // Bind VBOs
        if(*vboId1 > 0)
            render->bindVBO(M_VBO_ARRAY, *vboId1);

        if(indices) // If the SubMesh has indices
        {
            if(*vboId2 > 0) // If the indices are stored in the VBO
            {
              render->bindVBO(M_VBO_ELEMENT_ARRAY, *vboId2);
            }
        }

        // Set up vertex attribute
        int vertAttrib;
        render->getAttribLocation(fx, "Vertex", &vertAttrib);
        render->enableAttribArray(vertAttrib);

        render->setAttribPointer(vertAttrib, M_FLOAT, 3, NULL);

        // Set up normal attribute
        int normalAttrib;
        render->getAttribLocation(fx, "Normal", &normalAttrib);
        render->enableAttribArray(normalAttrib);

        int offset = sizeof(MVector3)*(mesh->getVerticesSize() + display->getBegin());
        render->setAttribPointer(normalAttrib, M_FLOAT, 3, (void*) offset);

        // Set up tangent attribute
        int tangentAttrib;
        render->getAttribLocation(fx, "Tangent", &tangentAttrib);
        render->enableAttribArray(tangentAttrib);

        offset += sizeof(MVector3)*mesh->getNormalsSize();
        render->setAttribPointer(tangentAttrib, M_FLOAT, 3, (void*) offset);

        // Set up texcoord attribute
        int texcoordAttrib = 0;
        if(texturePasses > 0 && texcoords != NULL)
        {
            // vert + normal + tang
            offset = sizeof(MVector3)*(mesh->getVerticesSize() + mesh->getNormalsSize() + mesh->getTangentsSize()+ display->getBegin());

            render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);

            render->setAttribPointer(texcoordAttrib, M_FLOAT, 2, (void*) offset);
            render->enableAttribArray(texcoordAttrib);
        }

        //render->disableAttribArray(vertAttrib);
        //render->disableAttribArray(texcoordAttrib);
       render->bindVAO(0);
       return;
    }

    fx = m_fx[0];
    render->bindFX(fx);

    render->bindVAO(*vao);

    if(texturePasses == 0)
    {
        // Tell the shader that we do not have any textures
        render->disableBlending();
    }
    else
    {
        render->enableBlending();
        render->setBlendingMode(M_BLENDING_ALPHA);
    }

    for(int i = 0; i < texturePasses; i++)
    {
        TexturePass* pass = material->getTexturePass(i);

        if(!pass || !pass->getTexture())
        {
            continue;
        }

        TextureRef* tex = pass->getTexture()->getTextureRef();
        render->bindTexture(tex->getTextureId(), i);
    }

    // Texture
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(fx, "Textures", texIds, 4);

    // Send the texture mode
    render->sendUniformInt(fx, "TextureMode", &texturePasses);

    // Set up modelview matrix
    MMatrix4x4 modelViewMatrix;
    MMatrix4x4 projectionMatrix;
    MMatrix4x4 modelMatrix;
    MMatrix4x4 normalMatrix;

    modelViewMatrix = *camera->getCurrentViewMatrix();
    projectionMatrix = *camera->getCurrentProjMatrix();

    // The modelview matrix holds the matrix of the current entity

    render->getModelViewMatrix(&modelMatrix);
    render->getProjectionMatrix(&projectionMatrix);
    modelViewMatrix = (modelViewMatrix * modelMatrix);
    normalMatrix = (modelViewMatrix.getInverse()).getTranspose();

    projectionMatrix = projectionMatrix * modelViewMatrix;

    // Send uniforms
    render->sendUniformMatrix(m_fx[0], "ProjModelViewMatrix", &projectionMatrix);
    render->sendUniformMatrix(m_fx[0], "ModelViewMatrix", &modelViewMatrix);

/*	float m_shininess;
    float m_customValue;
    MVector3 m_diffuse;
    MVector3 m_specular;
    MVector3 m_emit;
  */

    render->sendUniformVec3(m_fx[0], "Diffuse", material->getDiffuse());
    render->sendUniformVec3(m_fx[0], "Specular", material->getSpecular());
    render->sendUniformVec3(m_fx[0], "Emit", material->getEmit());

    float shininess = material->getShininess();
    render->sendUniformFloat(m_fx[0], "Shininess", &shininess);


    //modelViewMatrix.loadIdentity();
    //modelViewMatrix.setScale(MVector3(1,1,1));
    //modelViewMatrix.setRotationPartEuler(camera->getMatrix()->getEulerAngles());
    //modelViewMatrix = modelViewMatrix * camera->getMatrix()->getInverse().getTranspose();

    //modelViewMatrix.setScale(MVector3(0.5,0.5,0.5));

    // Set up normal matrix
    render->sendUniformMatrix(m_fx[0], "NormalMatrix", &normalMatrix);

    // Bind VBOs
    //if(*vboId1 > 0)
        //render->bindVBO(M_VBO_ARRAY, *vboId1);

    if(indices) // If the SubMesh has indices
    {
        if(*vboId2 > 0) // If the indices are stored in the VBO
        {
            render->bindVBO(M_VBO_ELEMENT_ARRAY, *vboId2);
            switch(indicesType)
            {
                case M_USHORT:
                    render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(short)));
                    break;
                case M_UINT:
                    render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(int)));
                    break;
                default:
                    MLOG_WARNING("Unsupported indices type!");
            }

            //render->bindVAO(*vao);
            //render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(short)));
            //render->drawArray(display->getPrimitiveType(), display->getBegin(), display->getSize());
            // MLOG_INFO("Draw stuff");

            // Clean up
            //render->bindVBO(M_VBO_ELEMENT_ARRAY, 0);
            //render->bindVBO(M_VBO_ARRAY, 0);
        }
        else // If the indices are not stored in the VBO
        {
            switch(indicesType)
            {
                case M_USHORT:
                    render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (unsigned short*)indices + display->getBegin());
                    break;
                case M_UINT:
                    render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (unsigned int*)indices + display->getBegin());
                    break;
                default:
                    MLOG_WARNING("Unsupported indices type!");
            }
        }
    }
    else // If we have no indices
        render->drawArray(display->getPrimitiveType(), display->getBegin(), display->getSize());

    // Clean up
    //render->bindVAO(0);
}

void DeferredRenderer::drawGBuffer(Scene* scene, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	MRenderingContext* render = engine->getRenderingContext();

	OEntity* entity;
	int num = scene->getEntitiesNumber();

	for(int i = 0; i < num; i++)
	{
        entity = scene->getEntityByIndex(i);

        if(entity->isVisible() && entity->isActive())
        {
            render->setMatrixMode(M_MATRIX_MODELVIEW);
            render->loadIdentity();
            render->multMatrix(entity->getMatrix());

            drawMesh(entity->getMesh(), camera);
        }
    }
}

void DeferredRenderer::init()
{
	std::string path;
	Level* level = NeoEngine::getInstance()->getLevel();
	NeoWindow* window = NeoWindow::getInstance();
    MRenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

    render->init();

	path = window->getCurrentDirectory();
	path += "/shaders/";
	path += SHADER_PATH;

	char file[512];

	getGlobalFilename(file, path.c_str(), defaultShaderFiles[0]);
	ShaderRef* vertShad = level->loadShader(file, M_SHADER_VERTEX);
	ShaderRef* fragShad;

	vertShad->update();

	for(int i = 1; i < DEFERRED_SHADERS; i++)
	{
		getGlobalFilename(file, path.c_str(), defaultShaderFiles[i]);
		fragShad = level->loadShader(file, M_SHADER_PIXEL);
		fragShad->update();

		m_shaders[i-1] = level->createFX(vertShad, fragShad);
            render->updateFX(m_shaders[i-1]->getFXId());

            m_shaders[i-1]->setImportant(true);

            m_fx[i-1] = m_shaders[i-1]->getFXId();
	}

    initFramebuffers();
    initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);

    // Start worker threads
    if(m_lightUpdateThread == NULL)
    {
        MThreadManager* tmgr = MThreadManager::getInstance();
        m_lightUpdateThread = tmgr->getNewThread();
        m_lightUpdateSemaphore = tmgr->getNewSemaphore();

        if(m_lightUpdateThread == NULL || m_lightUpdateSemaphore == NULL)
        {
            SAFE_DELETE(m_lightUpdateThread);
            SAFE_DELETE(m_lightUpdateSemaphore);
            return;
        }

        m_lightUpdateSemaphore->Init(1);
        m_lightUpdateThread->Start(DeferredRenderer::light_update_thread, "LightUpdate", this);
    }
}

inline int Pow2(int x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

void DeferredRenderer::clearFramebuffers()
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    if(m_gbufferTexID != 0)
        render->deleteTexture(&m_gbufferTexID);

    if(m_depthTexID != 0)
        render->deleteTexture(&m_depthTexID);

    if(m_framebufferID != 0)
        render->deleteFrameBuffer(&m_framebufferID);
}

void DeferredRenderer::initFramebuffers()
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
    MSystemContext * system = NeoEngine::getInstance()->getSystemContext();

    clearFramebuffers();

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    m_resolution = MVector2(screenWidth, screenHeight);

    // Update vertex cache
    m_vertices[0] = MVector2(0, 0);
    m_vertices[1] = MVector2(0, screenHeight);
    m_vertices[3] = MVector2(screenWidth, screenHeight);
    m_vertices[2] = MVector2(screenWidth, 0);

    //m_Resolution = Pow2(max(screenWidth, screenHeight));

    //if(m_ResolutionMultiplier == 0) m_ResolutionMultiplier = 1.0f;

    //m_Resolution *= m_ResolutionMultiplier;

    // create frame buffer
    render->createFrameBuffer(&m_framebufferID);

    // create render textures
    render->createTexture(&m_gbufferTexID);
    render->bindTexture(m_gbufferTexID);
    render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(M_WRAP_CLAMP);
    render->setTextureVWrapMode(M_WRAP_CLAMP);
    render->texImage(0, screenWidth, screenHeight, M_FLOAT, M_RGBA, 0);

    render->createTexture(&m_depthTexID);
    render->bindTexture(m_depthTexID);
    render->setTextureFilterMode(M_TEX_FILTER_NEAREST, M_TEX_FILTER_NEAREST);
    render->texImage(0, screenWidth, screenHeight, M_FLOAT, M_DEPTH, 0);

    render->createTexture(&m_positionTexID);
    render->bindTexture(m_positionTexID);
    render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(M_WRAP_CLAMP);
    render->setTextureVWrapMode(M_WRAP_CLAMP);
    render->texImage(0, screenWidth, screenHeight, M_FLOAT, M_RGBA, 0);

    /*render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(M_WRAP_CLAMP);
    render->setTextureVWrapMode(M_WRAP_CLAMP);
    render->texImage(0, screenWidth, screenHeight, M_FLOAT, M_RGBA, 0);*/

    render->createTexture(&m_normalTexID);
    render->bindTexture(m_normalTexID);
    render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(M_WRAP_CLAMP);
    render->setTextureVWrapMode(M_WRAP_CLAMP);
    render->texImage(0, screenWidth, screenHeight, M_FLOAT, M_RGBA, 0);
}

void DeferredRenderer::destroy(void)
{
    delete this;
}

const char* DeferredRenderer::getName(void)
{
	return getStaticName();
}

void DeferredRenderer::drawScene(Scene* scene, OCamera* camera)
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
    MSystemContext * system = NeoEngine::getInstance()->getSystemContext();

    m_currentScene = scene;

    unsigned int currentFrameBuffer = 0;
    render->getCurrentFrameBuffer(&currentFrameBuffer);

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    if(MVector2(screenWidth, screenHeight) != m_resolution)
    {
        initFramebuffers();
        initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);
    }

    // render to texture
    render->bindFrameBuffer(m_framebufferID);

    render->attachFrameBufferTexture(M_ATTACH_COLOR0, m_gbufferTexID);
    render->attachFrameBufferTexture(M_ATTACH_COLOR1, m_normalTexID);
    render->attachFrameBufferTexture(M_ATTACH_COLOR2, m_positionTexID);
    render->attachFrameBufferTexture(M_ATTACH_DEPTH, m_depthTexID);

    // Enable them to be drawn
    M_FRAME_BUFFER_ATTACHMENT buffers[3] = {M_ATTACH_COLOR0, M_ATTACH_COLOR1, M_ATTACH_COLOR2};
    render->setDrawingBuffers(buffers, 3);

    render->setViewport(0, 0, screenWidth, screenHeight); // change viewport

    MVector4 clearColor = camera->getClearColor();

    clearColor.w = 0.0;
    render->setClearColor(clearColor);
    render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

    drawGBuffer(scene, camera);

    // finish render to texture
    render->bindFrameBuffer(currentFrameBuffer);
    renderFinalImage(camera);
}

void DeferredRenderer::sendLight(unsigned int fx, OLight* l, int num, MMatrix4x4 matrix)
{
    MRenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

    char str[255];
    char ending[255];

    sprintf(str, "lights[%d].", num);
    strcpy(ending, str);

    strcat(ending, "Position");
    render->sendUniformVec3(fx, ending, matrix*l->getTransformedPosition());
}

void DeferredRenderer::renderFinalImage(OCamera* camera)
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
    MSystemContext * system = NeoEngine::getInstance()->getSystemContext();

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // draw the rendered textured with a shader effect
    //render->setClearColor(MVector3(1, 0, 0));
    render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

    set2D(screenWidth, screenHeight);

    render->bindFX(m_fx[1]);
    render->bindTexture(m_gbufferTexID);
    render->bindTexture(m_normalTexID, 1);
    render->bindTexture(m_depthTexID, 2);
    render->bindTexture(m_positionTexID, 3);
    render->disableBlending();

    // Send light data
    m_lightUpdateSemaphore->WaitAndLock();

    MMatrix4x4 camMat = *camera->getCurrentViewMatrix();
    camMat.setTranslationPart(MVector3(0,0,0));

    render->sendUniformInt(m_fx[1], "LightsCount", &m_numVisibleLights);
    for(int i = 0; i < m_numVisibleLights; i++)
    {
        sendLight(m_fx[1], m_visibleLights[i], i, camMat);
    }

    drawQuad(m_fx[1]);
    render->bindFX(0);
    m_lightUpdateSemaphore->Unlock();
}

void DeferredRenderer::drawText(OText* text)
{

}

void DeferredRenderer::initVBO(SubMesh * subMesh)
{
    NeoEngine * engine = NeoEngine::getInstance();
    MRenderingContext * render = engine->getRenderingContext();

    unsigned int* vboId1 = subMesh->getVBOid1();
    unsigned int* vboId2 = subMesh->getVBOid2();

    M_VBO_MODES mode = M_VBO_STATIC;
    if(subMesh->getSkinData() || subMesh->getMorphingData())
        mode = M_VBO_DYNAMIC;

    if(*vboId1 == 0 && mode == M_VBO_STATIC) // only use VBO for static geometry
    {
        // data
        MColor * colors = subMesh->getColors();
        MVector3 * vertices = subMesh->getVertices();
        MVector3 * normals = subMesh->getNormals();
        MVector3 * tangents = subMesh->getTangents();
        MVector2 * texCoords = subMesh->getTexCoords();

        unsigned int totalSize = sizeof(MVector3)*subMesh->getVerticesSize();
        if(normals)
            totalSize += sizeof(MVector3)*subMesh->getNormalsSize();
        if(tangents)
            totalSize += sizeof(MVector3)*subMesh->getTangentsSize();
        if(texCoords)
            totalSize += sizeof(MVector2)*subMesh->getTexCoordsSize();
        if(colors)
            totalSize += sizeof(MColor)*subMesh->getColorsSize();

        // indices
        M_TYPES indicesType = subMesh->getIndicesType();
        void * indices = subMesh->getIndices();

        // data VBO
        render->createVBO(vboId1);
        render->bindVBO(M_VBO_ARRAY, *vboId1);

        render->setVBO(M_VBO_ARRAY, 0, totalSize, mode);

        unsigned int offset = 0;
        render->setVBOSubData(M_VBO_ARRAY, offset, vertices, sizeof(MVector3)*subMesh->getVerticesSize());
        offset += sizeof(MVector3)*subMesh->getVerticesSize();

        if(normals)
        {
            render->setVBOSubData(M_VBO_ARRAY, offset, normals, sizeof(MVector3)*subMesh->getNormalsSize());
            offset += sizeof(MVector3)*subMesh->getNormalsSize();
        }

        if(tangents)
        {
            render->setVBOSubData(M_VBO_ARRAY, offset, tangents, sizeof(MVector3)*subMesh->getTangentsSize());
            offset += sizeof(MVector3)*subMesh->getTangentsSize();
        }

        if(texCoords)
        {
            render->setVBOSubData(M_VBO_ARRAY, offset, texCoords, sizeof(MVector2)*subMesh->getTexCoordsSize());
            offset += sizeof(MVector2)*subMesh->getTexCoordsSize();
        }

        if(colors)
        {
            render->setVBOSubData(M_VBO_ARRAY, offset, colors, sizeof(MColor)*subMesh->getColorsSize());
            offset += sizeof(MColor)*subMesh->getColorsSize();
        }

        // indices VBO
        if(indices)
        {
            unsigned int typeSize = indicesType == M_USHORT ? sizeof(short) : sizeof(int);

            render->createVBO(vboId2);
            render->bindVBO(M_VBO_ELEMENT_ARRAY, *vboId2);

            render->setVBO(M_VBO_ELEMENT_ARRAY, indices, subMesh->getIndicesSize()*typeSize, mode);
        }

        render->bindVBO(M_VBO_ARRAY, 0);
        render->bindVBO(M_VBO_ELEMENT_ARRAY, 0);
    }
}

void DeferredRenderer::set2D(unsigned int w, unsigned int h)
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
    render->setViewport(0, 0, w, h);

    // set ortho projection
    render->setMatrixMode(M_MATRIX_PROJECTION);
    render->loadIdentity();

    render->setOrthoView(0.0f, (float)w, (float)h, 0.0f, 1.0f, -1.0f);

    render->setMatrixMode(M_MATRIX_MODELVIEW);
    render->loadIdentity();
}

void DeferredRenderer::drawQuad(int fx)
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // screen size
    int screenWidth = 0;
    int screenHeight = 0;
    NeoEngine::getInstance()->getSystemContext()->getScreenSize((unsigned int*) &screenWidth, (unsigned int*) &screenHeight);

    // projmodelview matrix
    static MMatrix4x4 ProjMatrix;
    static MMatrix4x4 ModelViewMatrix;
    static MMatrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);
    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
    render->sendUniformMatrix(fx, "ProjModelViewMatrix", &ProjModelViewMatrix);

    // Texture
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(fx, "Textures", texIds, 4);

    // Width
    render->sendUniformInt(fx, "Width", &screenWidth, 1);
    // Height
    render->sendUniformInt(fx, "Height", &screenHeight, 1);

    // draw
    render->bindVAO(m_quadVAO);
    render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

void DeferredRenderer::initQuadVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx)
{
    MRenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

    if(*vao || *vbo || *texcoordVbo)
        clearQuadVAO(vao, vbo, texcoordVbo);

    render->createVAO(vao);
    render->bindVAO(*vao);

    render->createVBO(vbo);
    render->bindVBO(M_VBO_ARRAY, *vbo);
    render->setVBO(M_VBO_ARRAY, m_vertices, 4*sizeof(MVector2), M_VBO_STATIC);

    render->createVBO(texcoordVbo);
    render->bindVBO(M_VBO_ARRAY, *texcoordVbo);
    render->setVBO(M_VBO_ARRAY, m_texCoords, 4*sizeof(MVector2), M_VBO_STATIC);

    // Send Vertex data
    int vertexAttrib;
    int texcoordAttrib;

    // Vertex
    render->bindVBO(M_VBO_ARRAY, *vbo);
    render->getAttribLocation(fx, "Vertex", &vertexAttrib);
    render->enableAttribArray(vertexAttrib);
    render->setAttribPointer(vertexAttrib, M_FLOAT, 2, NULL);

    // TexCoord
    render->bindVBO(M_VBO_ARRAY, *texcoordVbo);
    render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
    render->enableAttribArray(texcoordAttrib);
    render->setAttribPointer(texcoordAttrib, M_FLOAT, 2, NULL);

    render->bindVAO(0);
}

void DeferredRenderer::clearQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo)
{
    MRenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

    render->deleteVAO(vao);
    render->deleteVBO(vbo);
    render->deleteVBO(texcoordVbo);
}

////////////////////////////////////////////////////////////////////////////////
//
// UPDATE THREADS
//
////////////////////////////////////////////////////////////////////////////////

int DeferredRenderer::light_update_thread(void* data)
{
   DeferredRenderer* self = (DeferredRenderer*) data;
   if(!self)
       return 1;

   NeoEngine* engine = NeoEngine::getInstance();
   NeoWindow* window = NeoWindow::getInstance();
   Scene* scene;
   OLight* light;

   while(engine->isActive())
   {
       self->m_lightUpdateSemaphore->WaitAndLock();
       scene = self->m_currentScene;

       if(scene == NULL)
       {
           self->m_lightUpdateSemaphore->Unlock();
           window->sleep(15);
           continue;
       }

       int j = 0;
       for(int i = 0; i < scene->getLightsNumber() && i < MAX_ENTITY_LIGHTS; i++)
       {
            if((light = scene->getLightByIndex(i))->isVisible() && light->isActive())
            {
                self->m_visibleLights[j] = light;
                j++;
            }
       }

       //MLOG_INFO("Num visible lights: " << j);
       self->m_numVisibleLights = j;
       self->m_lightUpdateSemaphore->Unlock();
       window->sleep(15);
   }

   return 0;
}

} /* namespace Neo */

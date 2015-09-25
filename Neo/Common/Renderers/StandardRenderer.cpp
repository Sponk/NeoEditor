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

#ifndef MOBILE_RENDERER

#include <glew.h>

#include <NeoEngine.h>
#include <StandardRenderer.h>
#include <Window/Window.h>
#include <string>
#include <algorithm>
#include <tinyutf8.h>

#ifndef SHADER_PATH
#define SHADER_PATH "./"
#endif

using namespace Neo;

/*
 * Additional scene data
 */
class SceneData : public Scene::AdditionalData
{
public:
	// FIXME: Check for sub meshes?
	std::vector<OEntity*> visibleEntities;
	Semaphore* visibilityLock;

	SceneData()
	{
		visibilityLock = ThreadFactory::getInstance()->getNewSemaphore();
		visibilityLock->Init(1);
	}

	~SceneData()
	{
		SAFE_DELETE(visibilityLock);
	}			   
};

/*
 * Additional Object3d data
 */
class CameraData : public Object3d::AdditionalData
{
public:
	// FIXME: Check for sub meshes?
	std::vector<OEntity*> visibleEntities;
	std::vector<OEntity*> visibleTransparentEntities;
	Semaphore* visibilityLock;

	Semaphore* lightLock;
	std::vector<OLight*> visibleLights;

	CameraData()
	{
		visibilityLock = ThreadFactory::getInstance()->getNewSemaphore();
		visibilityLock->Init(1);

		lightLock = ThreadFactory::getInstance()->getNewSemaphore();
		lightLock->Init(1);

		visibleLights.reserve(256);
		visibleEntities.reserve(256);
		visibleTransparentEntities.reserve(256);
	}

	~CameraData()
	{
		SAFE_DELETE(lightLock);
		SAFE_DELETE(visibilityLock);
	}
};

/*
 * Additional OEntity and OLight data
 */
class EntityData : public Object3d::AdditionalData
{
	unsigned int m_framebuffer;
	unsigned int m_texture[6];
	OCamera* m_camera[6];
	Object3d* m_parent;
	Scene* m_scene;
	StandardRenderer* m_renderer;

public:

	EntityData() :
		m_parent(nullptr),
		m_scene(nullptr)
	{

	}

	void drawCubemap()
	{
		NeoEngine* engine = NeoEngine::getInstance();
		StandardRenderer* renderer = static_cast<StandardRenderer*>(engine->getRenderer());
		RenderingContext* render = engine->getRenderingContext();
				
		render->bindFrameBuffer(m_framebuffer);
		render->attachFrameBufferTexture(ATTACH_COLOR0, m_texture[0]);

		// Enable them to be drawn
		//FRAME_BUFFER_ATTACHMENT buffers[1] = { ATTACH_COLOR0 };
		//render->setDrawingBuffers(buffers, 1);

		for (int i = 0; i < 6; i++)
			m_renderer->drawToTexture(m_scene, m_camera[i], 0);

		//MLOG_INFO("RENDER");
		render->bindFrameBuffer(0);

		//render->setViewport(0, 0, screenWidth, screenHeight); // change viewport
	}

	void initTextures(int res, Scene* scene, Object3d* parent)
	{
		if (m_scene) return;

		m_renderer = new StandardRenderer();
		RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
		m_parent = parent;
		m_scene = scene;

		m_renderer->smallInit(res, res);

		render->createFrameBuffer(&m_framebuffer);

		// GBuffer texture
		for (int i = 0; i < 6; i++)
		{
			render->createTexture(&m_texture[i]);
			render->bindTexture(m_texture[i]);
			render->setTextureFilterMode(TEX_FILTER_LINEAR, TEX_FILTER_LINEAR);
			render->setTextureUWrapMode(WRAP_CLAMP);
			render->setTextureVWrapMode(WRAP_CLAMP);
			render->texImage(0, res, res, VAR_FLOAT, TEX_RGBA, 0);

			m_camera[i] = scene->addNewCamera();
			m_camera[i]->linkTo(parent);
			m_camera[i]->setFov(90.0f);
		}

		m_camera[0]->rotate(Vector3(1, 0, 0), -90);
		m_camera[1]->rotate(Vector3(0, 1, 0), 90);
	
		m_camera[3]->rotate(Vector3(1, 0, 0), 90);
		m_camera[4]->rotate(Vector3(0, 1, 0), -90);
		m_camera[5]->rotate(Vector3(0, 1, 0), 180);
	}

	void bindCubemap()
	{

	}
};

static const char* defaultShaderFiles[NUM_SHADERS] = {"Default.vert", "Forward.frag", "Final.frag", "Shadow.frag"};
static const char* subroutines[5] = { "cookModelColor", "cookModelDiffuse", "cookModelDiffuseSpecular", "cookModelDiffuseNormal", "cookModelDiffuseNormalSpecular"};
//static const char* subroutines[5] = { "phongModelColor", "phongModelDiffuse", "phongModelDiffuseSpecular", "phongModelDiffuseNormal", "phongModelDiffuseNormalSpecular" };

StandardRenderer::StandardRenderer()
{
	// Let's hope we have an GL context
	for(int i = 0; i < NUM_SHADERS; i++)
	{
		m_shaders[i] = NULL;
		m_fx[i] = -1;
	}

	m_framebufferID = 0;
	m_finalFramebufferID = 0;
	m_gbufferTexID = 0;
	m_depthTexID = 0;
	m_finalTexID = 0;

	m_quadTexCoordVBO = 0;
	m_quadVAO = 0;
	m_quadVBO = 0;

	m_lightUpdateThread = NULL;
	m_lightUpdateSemaphore = NULL;
	m_currentScene = NULL;
	m_numVisibleLights = 0;

	m_texCoords[0] = Vector2(0, 1);
	m_texCoords[1] = Vector2(0, 0);
	m_texCoords[3] = Vector2(1, 0);
	m_texCoords[2] = Vector2(1, 1);

	init();
}

StandardRenderer::~StandardRenderer()
{

}

void StandardRenderer::drawMesh(Mesh* mesh, OCamera* camera, Material* materials, bool wireframe)
{
	int num = mesh->getSubMeshsNumber();
	SubMesh* subMeshes = mesh->getSubMeshs();
	Armature* armature = mesh->getArmature();

	for(int i = 0; i < num; i++)
	{
		SkinData* skinData = subMeshes[i].getSkinData();
		
		if (armature && skinData)
		{
			unsigned int verticesSize = subMeshes[i].getVerticesSize();
			Vector3* skinVertices = subMeshes[i].getSkinVertices();
			Vector3* skinNormals = subMeshes[i].getSkinNormals();
			Vector3* skinTangents = subMeshes[i].getSkinTangents();
						
			computeSkinning(armature, skinData, subMeshes[i].getVertices(), subMeshes[i].getNormals(),
								subMeshes[i].getTangents(), skinVertices, skinNormals, skinTangents);

			subMeshes[i].getBoundingBox()->initFromPoints(skinVertices, verticesSize);
		}

		drawSubMesh(&subMeshes[i], camera, materials, wireframe);
	}

	mesh->updateBoundingBox();
}

void StandardRenderer::drawSubMesh(SubMesh* mesh, OCamera* camera, Material* materials, bool wireframe)
{
	NeoEngine* engine = NeoEngine::getInstance();

	initVBO(mesh);

	for(int i = 0; i < mesh->getDisplaysNumber(); i++)
	{
		drawDisplay(mesh, mesh->getDisplay(i), camera, materials, wireframe);
	}
}

void StandardRenderer::drawDisplay(SubMesh* mesh, MaterialDisplay* display, OCamera* camera, Material* materials, bool wireframe)
{
	NeoEngine *engine = NeoEngine::getInstance();
	RenderingContext *render = engine->getRenderingContext();

	// Retrieve VBO information
	unsigned int *vboId1 = mesh->getVBOid1();
	unsigned int *vboId2 = mesh->getVBOid2();
	unsigned int *vao = display->getVAO();

	// Save data into variables
	Vector2 *texcoords = mesh->getTexCoords();
	VAR_TYPES indicesType = mesh->getIndicesType();
	void *indices = mesh->getIndices();

	Material *material = &materials[display->getMaterialId()];
	int texturePasses = material->getTexturesPassNumber();

	int fx = 0;
	if (*vao == 0) {
		prepareMaterialDisplay(mesh, display);
		return;
	}

	fx = m_fx[0];
	render->bindFX(fx);
	render->bindVAO(*vao);

	if (texturePasses == 0) {
		// Tell the shader that we do not have any textures
		render->disableBlending();
	}
	else {
		render->disableBlending();
	}

	for (int i = 0; i < texturePasses; i++) {
		TexturePass *pass = material->getTexturePass(i);

		if (!pass || !pass->getTexture()) {
			continue;
		}

		TextureRef *tex = pass->getTexture()->getTextureRef();
		render->bindTexture(tex->getTextureId(), i);
	}

	// Texture
	int texIds[4] = {0, 1, 2, 3};
	render->sendUniformInt(fx, "Textures", texIds, 4);

	// Send the texture mode
	render->sendUniformInt(fx, "TextureMode", &texturePasses);
	render->selectSubroutine(fx, M_SHADER_PIXEL, subroutines[texturePasses]);

	// Set up modelview matrix
	Matrix4x4 modelViewMatrix;
	Matrix4x4 modelViewProjectionMatrix;
	Matrix4x4 modelMatrix;
	Matrix4x4 normalMatrix;

	render->getModelViewMatrix(&modelMatrix);

	camera->enable();
	modelViewMatrix = *camera->getCurrentViewMatrix() * modelMatrix;
	modelViewProjectionMatrix = *camera->getCurrentProjMatrix() * modelViewMatrix;

	normalMatrix = modelViewMatrix.getInverse().getTranspose();

	// Send uniforms
	render->sendUniformMatrix(m_fx[0], "ProjModelViewMatrix", &modelViewProjectionMatrix);
	render->sendUniformMatrix(m_fx[0], "ModelViewMatrix", &modelViewMatrix);
	render->sendUniformVec3(m_fx[0], "Diffuse", material->getDiffuse());
	render->sendUniformVec3(m_fx[0], "Specular", material->getSpecular());
	render->sendUniformVec3(m_fx[0], "Emit", material->getEmit());

	int hasTransparency = (mesh->hasTransparency()) ? 1 : 0;
	float opacity = material->getOpacity();

	render->sendUniformInt(m_fx[0], "HasTransparency", &hasTransparency);

	if (hasTransparency) {
		render->enableBlending();
		render->setBlendingMode(material->getBlendMode());
		render->disableCullFace();
		//render->setDepthMask(true);

		sendLights(m_fx[0], camera);

		render->sendUniformFloat(m_fx[0], "Opacity", &opacity);
	}
	else
	{
		render->setDepthMask(true);
		render->enableCullFace();
	}

	float shininess = material->getShininess();
	render->sendUniformFloat(m_fx[0], "Shininess", &shininess);

	// Set up normal matrix
	render->sendUniformMatrix(m_fx[0], "NormalMatrix", &normalMatrix);

	// Set cull mode
	render->setCullMode(display->getCullMode());
	render->enableDepthTest();

	if(indices) // If the SubMesh has indices
	{
		if(*vboId2 > 0) // If the indices are stored in the VBO
		{
			render->bindVBO(VBO_ELEMENT_ARRAY, *vboId2);
			switch(indicesType)
			{
				case VAR_USHORT:
					render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(short)));
					if(wireframe)
					{
						render->disableBlending();
						render->sendUniformVec3(m_fx[0], "Diffuse", Vector3(0.1,0.1,0.1));
						render->enablePolygonOffset(-5.0,-5.0);
						//render->disableDepthTest();

						render->drawElement(PRIMITIVE_LINES, display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(short)));
						render->disablePolygonOffset();
					}
					break;
				case VAR_UINT:
					render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(int)));
					if(wireframe)
					{
						//render->disableDepthTest();
						render->drawElement(PRIMITIVE_LINES, display->getSize(), indicesType, (void*)(display->getBegin()*sizeof(short)));
					}
					break;
				default:
					MLOG_WARNING("Unsupported indices type!");
			}
		}
		else // If the indices are not stored in the VBO
		{
			switch(indicesType)
			{
				case VAR_USHORT:
					render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (unsigned short*)indices + display->getBegin());
					if(wireframe)
					{
						//render->disableDepthTest();
						render->drawElement(PRIMITIVE_LINES, display->getSize(), indicesType, (unsigned short*)indices + display->getBegin());
					}
					break;
			case VAR_UINT:
					render->drawElement(display->getPrimitiveType(), display->getSize(), indicesType, (unsigned int*) indices + display->getBegin());
					if (wireframe)
					{
						// render->disableDepthTest();
						render->drawElement(PRIMITIVE_LINES, display->getSize(), indicesType, (unsigned short*) indices + display->getBegin());
					}
					break;
				default:
					MLOG_WARNING("Unsupported indices type!");
			}
		}
	}
	else // If we have no indices
		render->drawArray(display->getPrimitiveType(), display->getBegin(), display->getSize());
}

void StandardRenderer::drawGBuffer(Scene* scene, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	render->setDepthMask(true);
	
	OEntity* entity;
	Mesh* mesh;
	
	// Lock scene so we don't have a surprise
	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());

	if(!data) return;

	data->visibilityLock->WaitAndLock();

	for (OEntity* entity : data->visibleEntities)
	{
		if (entity->isVisible() && entity->isActive() && !entity->isInvisible())
		{			
			render->setMatrixMode(MATRIX_MODELVIEW);
			render->loadIdentity();
			render->multMatrix(entity->getMatrix());
						
			mesh = entity->getMesh();

			// Update Animation
			if (mesh->getArmatureAnim())
			{
				animateArmature(
					mesh->getArmature(),
					mesh->getArmatureAnim(),
					entity->getCurrentFrame()
					);
			}
			else if (mesh->getArmature())
			{
				mesh->getArmature()->processBonesLinking();
				mesh->getArmature()->updateBonesSkinMatrix();
			}

			if (mesh->getMaterialsAnim())
				animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());

			if (mesh->getTexturesAnim())
				animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

			drawMesh(entity->getMesh(), camera, entity->getMaterial(), entity->hasWireframe());
		}
	}

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();
	data->visibilityLock->Unlock();
}


void StandardRenderer::drawTransparents(Scene* scene, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	OEntity* entity;
	Mesh* mesh;
	
	// Lock scene so we don't have a surprise
	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());

	if(!data) return;

	data->visibilityLock->WaitAndLock();

	for (OEntity* entity : data->visibleTransparentEntities)
	{
		if (entity->isVisible() && entity->isActive() && !entity->isInvisible())
		{			
			render->setMatrixMode(MATRIX_MODELVIEW);
			render->loadIdentity();
			render->multMatrix(entity->getMatrix());
						
			mesh = entity->getMesh();

			// Update Animation
			if (mesh->getArmatureAnim())
			{
				animateArmature(
					mesh->getArmature(),
					mesh->getArmatureAnim(),
					entity->getCurrentFrame()
					);
			}
			else if (mesh->getArmature())
			{
				mesh->getArmature()->processBonesLinking();
				mesh->getArmature()->updateBonesSkinMatrix();
			}

			if (mesh->getMaterialsAnim())
				animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());

			if (mesh->getTexturesAnim())
				animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

			drawMesh(entity->getMesh(), camera, entity->getMaterial(), entity->hasWireframe());
		}
	}

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();

	int num = scene->getTextsNumber();
	for (int i = 0; i < num; i++)
	{
		drawText(scene->getTextByIndex(i), camera);
	}

	data->visibilityLock->Unlock();
}

void StandardRenderer::init()
{
	std::string path;
	Level* level = NeoEngine::getInstance()->getLevel();
	NeoWindow* window = NeoWindow::getInstance();
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	// Do not initialize twice
	if(m_fx[0] != -1 && m_fx[1] != -1)
		return;

	MLOG_INFO("Initializing the rendering engine");
	render->init();

	path = window->getCurrentDirectory();
	path += "/";
	path += SHADER_PATH;

	char file[512];

	getGlobalFilename(file, path.c_str(), defaultShaderFiles[0]);
	ShaderRef* vertShad = level->loadShader(file, M_SHADER_VERTEX);
	ShaderRef* fragShad;

	vertShad->update();

	for(int i = 1; i < NUM_SHADERS; i++)
	{
		MLOG_INFO("Loading shader: " << defaultShaderFiles[i]);
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
	initTextVAO(&m_textVAO, &m_textVBO, &m_textTexCoordVBO, m_fx[0]);

	// Start worker threads
	if(m_lightUpdateThread == NULL)
	{
		ThreadFactory* tmgr = ThreadFactory::getInstance();
		m_lightUpdateThread = tmgr->getNewThread();

		m_lightUpdateSemaphore = tmgr->getNewSemaphore();
		m_visibilityUpdateSemaphore = tmgr->getNewSemaphore();

		m_visibilityThread = tmgr->getNewThread();

		if(m_lightUpdateThread == NULL || m_lightUpdateSemaphore == NULL || m_visibilityThread == NULL)
		{
			MLOG_INFO("Could not create update threads!");
			SAFE_DELETE(m_lightUpdateThread);
			SAFE_DELETE(m_lightUpdateSemaphore);
			SAFE_DELETE(m_visibilityUpdateSemaphore);
			SAFE_DELETE(m_visibilityThread);
			return;
		}

		m_lightUpdateSemaphore->Init(1);
		m_visibilityUpdateSemaphore->Init(1);

		m_lightUpdateThread->Start(StandardRenderer::light_update_thread, "LightUpdate", this);
		m_visibilityThread->Start(StandardRenderer::visibility_thread_mainscene, "VisUpdate", this);
	}
}

void StandardRenderer::smallInit(unsigned int width, unsigned int height)
{
	std::string path;
	Level* level = NeoEngine::getInstance()->getLevel();
	NeoWindow* window = NeoWindow::getInstance();
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	render->init();

	path = window->getCurrentDirectory();
	path += "/";
	path += SHADER_PATH;

	char file[512];

	getGlobalFilename(file, path.c_str(), defaultShaderFiles[0]);
	ShaderRef* vertShad = level->loadShader(file, M_SHADER_VERTEX);
	ShaderRef* fragShad;

	vertShad->update();

	for (int i = 1; i < NUM_SHADERS; i++)
	{
		getGlobalFilename(file, path.c_str(), defaultShaderFiles[i]);
		fragShad = level->loadShader(file, M_SHADER_PIXEL);
		fragShad->update();

		m_shaders[i - 1] = level->createFX(vertShad, fragShad);
		render->updateFX(m_shaders[i - 1]->getFXId());

		m_shaders[i - 1]->setImportant(true);
		m_fx[i - 1] = m_shaders[i - 1]->getFXId();
	}

	initFramebuffers(Vector2(width, height));
	initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);
	initTextVAO(&m_textVAO, &m_textVBO, &m_textTexCoordVBO, m_fx[0]);
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

void StandardRenderer::clearFramebuffers()
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

	if(m_gbufferTexID != 0)
		render->deleteTexture(&m_gbufferTexID);

	if(m_depthTexID != 0)
		render->deleteTexture(&m_depthTexID);

	if(m_framebufferID != 0)
		render->deleteFrameBuffer(&m_framebufferID);
}

void StandardRenderer::initFramebuffers(Vector2 res)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	SystemContext * system = NeoEngine::getInstance()->getSystemContext();

	clearFramebuffers();

	// screen size
	if (res.x == 0 || res.y == 0)
	{
		unsigned int screenWidth = 0;
		unsigned int screenHeight = 0;
		system->getScreenSize(&screenWidth, &screenHeight);
		res = Vector2(screenWidth, screenHeight);
	}

	m_resolution = Vector2(res.x, res.y);

	// Update vertex cache
	m_vertices[0] = Vector2(0, 0);
	m_vertices[1] = Vector2(0, res.y);
	m_vertices[2] = Vector2(res.x, 0);
	m_vertices[3] = Vector2(res.x, res.y);

	// create frame buffer
	render->createFrameBuffer(&m_framebufferID);

	// create render textures
	// GBuffer texture
	render->createTexture(&m_gbufferTexID);
	render->bindTexture(m_gbufferTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	// Depth texture
	render->createTexture(&m_depthTexID);
	render->bindTexture(m_depthTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->texImage(0, res.x, res.y, VAR_UBYTE, TEX_DEPTH, 0);

	// Position texture
	render->createTexture(&m_positionTexID);
	render->bindTexture(m_positionTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	// Normal texture
	render->createTexture(&m_normalTexID);
	render->bindTexture(m_normalTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	// Data texture
	render->createTexture(&m_dataTexID);
	render->bindTexture(m_dataTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	render->bindFrameBuffer(m_framebufferID);

	render->attachFrameBufferTexture(ATTACH_COLOR0, m_gbufferTexID);
	render->attachFrameBufferTexture(ATTACH_COLOR1, m_normalTexID);
	render->attachFrameBufferTexture(ATTACH_COLOR2, m_positionTexID);
	render->attachFrameBufferTexture(ATTACH_COLOR3, m_dataTexID);
	render->attachFrameBufferTexture(ATTACH_DEPTH, m_depthTexID);

	// Enable them to be drawn
	FRAME_BUFFER_ATTACHMENT buffers[5] = {ATTACH_COLOR0, ATTACH_COLOR1, ATTACH_COLOR2, ATTACH_COLOR3, ATTACH_DEPTH};
	render->setDrawingBuffers(buffers, 4);
	render->bindFrameBuffer(0);

	// Prepare post process fbo
	render->createFrameBuffer(&m_finalFramebufferID);
	render->bindFrameBuffer(m_finalFramebufferID);

	render->createTexture(&m_finalTexID);
	render->bindTexture(m_finalTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	render->attachFrameBufferTexture(ATTACH_COLOR0, m_finalTexID);

	FRAME_BUFFER_ATTACHMENT finalbuffers[1] = {ATTACH_COLOR0};
	render->setDrawingBuffers(finalbuffers, 1);

	render->bindFrameBuffer(0);
}

void StandardRenderer::destroy(void)
{
	delete this;
}

void StandardRenderer::prepareMaterialDisplay(SubMesh* mesh, MaterialDisplay* display)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	
	unsigned int* vao = display->getVAO();
	unsigned int* vboId1 = mesh->getVBOid1();
	unsigned int* vboId2 = mesh->getVBOid2();
	void* indices = mesh->getIndices();
	Vector2* texcoords = mesh->getTexCoords();

	render->bindFX(m_fx[0]);

	if (*vao == 0)
		render->createVAO(vao);
	
	render->bindVAO(*vao);

	// Bind VBOs
	if (*vboId1 > 0)
		render->bindVBO(VBO_ARRAY, *vboId1);

	if (indices) // If the SubMesh has indices
	{
		if (*vboId2 > 0) // If the indices are stored in the VBO
		{
			render->bindVBO(VBO_ELEMENT_ARRAY, *vboId2);
		}
	}

	// Set up vertex attribute
	int vertAttrib;
	int offset = 0;

	render->getAttribLocation(m_fx[0], "Vertex", &vertAttrib);
	render->enableAttribArray(vertAttrib);
	render->setAttribPointer(vertAttrib, VAR_FLOAT, 3, NULL);

	offset = sizeof(Vector3)*(mesh->getVerticesSize() + display->getBegin());

	// Set up normal attribute
	int normalAttrib;
	render->getAttribLocation(m_fx[0], "Normal", &normalAttrib);
	render->enableAttribArray(normalAttrib);
	render->setAttribPointer(normalAttrib, VAR_FLOAT, 3, (void*)offset);

	offset += sizeof(Vector3)*mesh->getNormalsSize();

	// Set up tangent attribute
	int tangentAttrib;
	render->getAttribLocation(m_fx[0], "Tangent", &tangentAttrib);
	render->enableAttribArray(tangentAttrib);
	render->setAttribPointer(tangentAttrib, VAR_FLOAT, 3, (void*)offset);

	offset += sizeof(Vector3)*mesh->getTangentsSize();

	// Set up color attribute
	int colorAttrib;
	render->getAttribLocation(m_fx[0], "Color", &colorAttrib);
	render->enableAttribArray(colorAttrib);
	render->setAttribPointer(colorAttrib, VAR_FLOAT, 3, (void*)offset);

	// Set up texcoord attribute
	int texcoordAttrib = 0;
	if (display->getMaterial()->getTexturesPassNumber() > 0 && texcoords != NULL)
	{
		// vert + normal + tang
		offset = sizeof(Vector3)*(mesh->getVerticesSize() + mesh->getNormalsSize() + mesh->getTangentsSize() + display->getBegin());

		render->getAttribLocation(m_fx[0], "TexCoord", &texcoordAttrib);
		render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, (void*)offset);
		render->enableAttribArray(texcoordAttrib);
	}

	render->bindVAO(0);
}


void StandardRenderer::drawToTexture(Scene * scene, OCamera* camera, unsigned int texId)
{
}

void StandardRenderer::drawScene(Scene* scene, OCamera* camera)
{
	/// FIXME: MAKE IT SWITCHABLE!
	// Lock scene so we don't have a surprise
/*	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());

	if (!data) return;

	data->visibilityLock->WaitAndLock();

	for (OEntity* entity : data->visibleEntities)
	{
		if (entity->isVisible() && entity->isActive() && !entity->isInvisible())
		{
			EntityData* edata = static_cast<EntityData*>(entity->getAdditionalData());
			if (!edata)
			{
				edata = new EntityData();
				edata->initTextures(256, scene, entity);
				entity->setAdditionalData(edata);
			}

			edata->drawCubemap();
		}
	}
	data->visibilityLock->Unlock();*/

	//drawToTexture(scene, camera, m_gbufferTexID);
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	SystemContext * system = NeoEngine::getInstance()->getSystemContext();

	m_currentScene = scene;

	unsigned int currentFrameBuffer = 0;
	render->getCurrentFrameBuffer(&currentFrameBuffer);

	// screen size
	unsigned int screenWidth = 0;
	unsigned int screenHeight = 0;
	system->getScreenSize(&screenWidth, &screenHeight);

	if(Vector2(screenWidth, screenHeight) != m_resolution)
	{
		initFramebuffers();
		initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);
	}
		
	// render to texture
	render->bindFrameBuffer(m_framebufferID);
	render->setViewport(0, 0, screenWidth, screenHeight); // change viewport

	Vector4 clearColor = camera->getClearColor();

	clearColor.w = 0.0;
	render->setClearColor(clearColor);
	render->clear(BUFFER_COLOR | BUFFER_DEPTH);
	
	// Send some common data
	Vector3 ambientLight = scene->getAmbientLight();
	render->sendUniformVec3(m_fx[1], "AmbientLight", ambientLight);

	sendLights(m_fx[1], camera);
	drawGBuffer(scene, camera);

	// finish render to texture
	render->bindFrameBuffer(m_finalFramebufferID);
	renderFinalImage(scene, camera);

	render->enableDepthTest();
	drawTransparents(scene, camera);

	render->bindFrameBuffer(currentFrameBuffer);
	renderFinalImage(scene, camera, true);
}


void StandardRenderer::sendLights(unsigned int shader, OCamera* camera)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	
	render->bindFX(shader);

	Matrix4x4 camMat;
	camera->enable();
	camMat = *camera->getCurrentViewMatrix();

	// Send light data
	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());
	if(!data) return;

    data->lightLock->WaitAndLock();

    int num = data->visibleLights.size();
	render->sendUniformInt(shader, "LightsCount", &num);

	int i = 0;
	for (OLight* l : data->visibleLights)
	{
		sendLight(shader, l, i++, camMat);
	}
	data->lightLock->Unlock();
}

void StandardRenderer::sendLight(unsigned int fx, OLight* l, int num, Matrix4x4 matrix)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	char str[255];
	char ending[255];

	sprintf(str, "lights[%d].", num);

	strcpy(ending, str);
	strcat(ending, "Position");
	Vector3 position = matrix * l->getTransformedPosition();
	render->sendUniformVec3(fx, ending, position);

	strcpy(ending, str);
	strcat(ending, "Diffuse");
	render->sendUniformVec3(fx, ending, l->getFinalColor());

	float intensity = l->getIntensity();
	strcpy(ending, str);
	strcat(ending, "Intensity");
	render->sendUniformFloat(fx, ending, &intensity);

	float radius = l->getRadius();
	strcpy(ending, str);
	strcat(ending, "Radius");
	render->sendUniformFloat(fx, ending, &radius);

	float spotAngle = cosf(l->getSpotAngle()*DEG_TO_RAD);
	strcpy(ending, str);
	strcat(ending, "SpotCos");
	render->sendUniformFloat(fx, ending, &spotAngle);

	float spotExponent = l->getSpotExponent();
	strcpy(ending, str);
	strcat(ending, "SpotExp");
	render->sendUniformFloat(fx, ending, &spotExponent);

	strcpy(ending, str);
	strcat(ending, "SpotDir");
	Vector3 spotDirection = matrix.getRotatedVector3(l->getRotatedVector(Vector3(0, 0, -1))).getNormalized();
	render->sendUniformVec3(fx, ending, spotDirection);

	float quadraticAttenuation = 0.0;
	// attenuation
	//if (l->getSpotAngle() > 0.0f)
	//{
		quadraticAttenuation = (8.0f / l->getRadius());
		quadraticAttenuation = (quadraticAttenuation*quadraticAttenuation)*l->getIntensity();
	//}

	strcpy(ending, str);
	strcat(ending, "QuadraticAttenuation");
	render->sendUniformFloat(fx, ending, &quadraticAttenuation);

	// Constant attenuation = 1
	float attenuation = 1.0;
	strcpy(ending, str);
	strcat(ending, "ConstantAttenuation");
	render->sendUniformFloat(fx, ending, &attenuation);

	// Linear attenuation = 0
	attenuation = 0.0;
	strcpy(ending, str);
	strcat(ending, "LinearAttenuation");
	render->sendUniformFloat(fx, ending, &attenuation);
}

void StandardRenderer::renderFinalImage(Scene* scene, OCamera* camera, bool postFx)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	SystemContext * system = NeoEngine::getInstance()->getSystemContext();

	// screen size
	unsigned int screenWidth = 0;
	unsigned int screenHeight = 0;
	system->getScreenSize(&screenWidth, &screenHeight);

	// draw the rendered textured with a shader effect
	set2D(screenWidth, screenHeight);

	render->bindFX(m_fx[1]);
	int postEffects = 1;

	if(!postFx)
	{
		postEffects = 0;
		render->bindTexture(m_gbufferTexID);
		render->bindTexture(m_normalTexID, 1);
		render->bindTexture(m_positionTexID, 2);
		render->bindTexture(m_dataTexID, 3);

		render->enableBlending();
		render->setBlendingMode(BLENDING_ALPHA);

		render->clear(BUFFER_COLOR);
	}
	else
	{
		render->enableBlending();
		render->bindTexture(m_finalTexID);
	}

	render->sendUniformInt(m_fx[1], "PostEffects", &postEffects, 1);
	render->bindTexture(m_depthTexID, 4);

	// Set cull mode
	render->setCullMode(CULL_BACK);
	render->disableDepthTest();
	render->setDepthMask(false);
	
	drawQuad(m_fx[1]);

	render->bindFX(0);
	render->enableDepthTest();
	render->setDepthMask(true);
}

void StandardRenderer::drawText(OText* textObj, OCamera* camera)
{
	const char* text = textObj->getText();

	if(!text || strlen(text) == 0)
		return;

	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	Font* font = textObj->getFont();

	Matrix4x4 modelViewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 mvp;

	if (camera != nullptr)
	{
		modelViewMatrix =
			(*camera->getCurrentViewMatrix()) * (*textObj->getMatrix());
		projectionMatrix = *camera->getCurrentProjMatrix();
	}
	else
	{
		render->getModelViewMatrix(&modelViewMatrix);
		render->getProjectionMatrix(&projectionMatrix);
	}

	mvp = projectionMatrix * modelViewMatrix;

	render->bindFX(m_fx[0]);
	render->bindVAO(m_textVAO);

	render->selectSubroutine(m_fx[0], M_SHADER_PIXEL, "TextShader");
	render->bindTexture(font->getTextureId());

	render->enableBlending();
	render->setBlendingMode(BLENDING_ALPHA);
	render->enableDepthTest();
	//render->disableCullFace();

	int uniform = 1;
	render->sendUniformInt(m_fx[0], "HasTransparency", &uniform);

	float opacity = textObj->getColor().w;
	render->sendUniformFloat(m_fx[0], "Opacity", &opacity);

	uniform = -1;
	render->sendUniformInt(m_fx[0], "TextureMode", &uniform);

	int texIds[4] = { 0, 1, 2, 3 };
	render->sendUniformInt(m_fx[0], "Textures", texIds, 4);
	render->sendUniformMatrix(m_fx[0], "ProjModelViewMatrix", &mvp);

	render->sendUniformVec3(m_fx[0], "Diffuse", textObj->getColor());

	Character* c;
	Vector2 scale;
	Vector2 pos;

	float fontSize = font->getFontSize();
	float size = textObj->getSize();
	float tabsize = size * 2.0f;
    float widthFactor = font->getTextureWidth() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;

	float xpos = 0;
	float ypos = 0;

	Vector2 texCoords[4];
	Vector2 vertices[4];
	Vector2 offset;

	vector<float>* linesOffset = textObj->getLinesOffset();

	if (!linesOffset || linesOffset->size() == 0)
		return;

	float lineOffset = (*linesOffset)[0];
	int line = 0;
	
	/*std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf16conv;
    std::u32string unicodeStr = utf16conv.from_bytes(text);
	length = unicodeStr.length();*/

	unsigned int state = 0;
	unsigned int character;
	unsigned char* s = (unsigned char*) text;
	
	for(; *s; s++)
	{
		if(utf8_decode(&state, &character, *s) != UTF8_ACCEPT)
			continue;

		if (character == '\n')
		{
			line++;
			lineOffset = (*linesOffset)[line];

			xpos = 0;
			ypos += size;
			continue;
		}

		if(character == '\t')
		{
			unsigned int tab = static_cast<unsigned int>(xpos / tabsize) + 1;
			xpos = tab*tabsize;
			continue;
		}
		
		c = font->getCharacter(character);

		if (!c)
			continue;

		pos = c->getPos();
		scale = c->getScale();
		offset = c->getOffset() * size;

		// Round to keep the text pixel aligned when doing
		// 2D rendering. Should not affect 3D text much.
		offset.x += floor(lineOffset);

		texCoords[0] = Vector2(pos.x, pos.y + scale.y);
		texCoords[1] = Vector2(pos.x + scale.x, pos.y + scale.y);
		texCoords[2] = Vector2(pos.x, pos.y);
		texCoords[3] = Vector2(pos.x + scale.x, pos.y);

		float width = scale.x * widthFactor * size;
		float height = scale.y * heightFactor * size;

		vertices[0] = Vector2(xpos, ypos + height) + offset;
		vertices[1] = Vector2(xpos + width, ypos + height) + offset;
		vertices[2] = Vector2(xpos, ypos) + offset;
		vertices[3] = Vector2(xpos + width, ypos) + offset;

		// TexCoord
		render->bindVBO(VBO_ARRAY, m_textTexCoordVBO);
		render->setVBO(VBO_ARRAY, texCoords, 4 * sizeof(Vector2), VBO_DYNAMIC);

		// Vertices
		render->bindVBO(VBO_ARRAY, m_textVBO);
		render->setVBO(VBO_ARRAY, vertices, 4 * sizeof(Vector2), VBO_DYNAMIC);

		render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);
		xpos += c->getXAdvance() * size;
	}

	render->bindVAO(0);
}

void StandardRenderer::initVBO(SubMesh * subMesh)
{
	NeoEngine * engine = NeoEngine::getInstance();
	RenderingContext * render = engine->getRenderingContext();

	unsigned int* vboId1 = subMesh->getVBOid1();
	unsigned int* vboId2 = subMesh->getVBOid2();

	VBO_MODES mode = VBO_STATIC;
	if(subMesh->getSkinData() || subMesh->getMorphingData())
		mode = VBO_DYNAMIC;

	if(*vboId1 == 0 || mode == VBO_DYNAMIC)
	{
		// data
		Color * colors = subMesh->getColors();
		
		Vector3 * vertices = subMesh->getVertices();
		Vector3 * normals = subMesh->getNormals();
		Vector3 * tangents = subMesh->getTangents();

		Vector2 * texCoords = subMesh->getTexCoords();

		if (mode == VBO_DYNAMIC)
		{
			vertices = subMesh->getSkinVertices();
			normals = subMesh->getSkinNormals();
			tangents = subMesh->getSkinTangents();
		}

		unsigned int totalSize = sizeof(Vector3)*subMesh->getVerticesSize();
		if(normals)
			totalSize += sizeof(Vector3)*subMesh->getNormalsSize();
		if(tangents)
			totalSize += sizeof(Vector3)*subMesh->getTangentsSize();
		if(texCoords)
			totalSize += sizeof(Vector2)*subMesh->getTexCoordsSize();
		if(colors)
			totalSize += sizeof(Color)*subMesh->getColorsSize();

		// indices
		VAR_TYPES indicesType = subMesh->getIndicesType();
		void * indices = subMesh->getIndices();

		// data VBO
		if (*vboId1 == 0)
			render->createVBO(vboId1);
		
		render->bindVBO(VBO_ARRAY, *vboId1);
		render->setVBO(VBO_ARRAY, 0, totalSize, mode);

		unsigned int offset = 0;
		render->setVBOSubData(VBO_ARRAY, offset, vertices, sizeof(Vector3)*subMesh->getVerticesSize());
		offset += sizeof(Vector3)*subMesh->getVerticesSize();

		if(normals)
		{
			render->setVBOSubData(VBO_ARRAY, offset, normals, sizeof(Vector3)*subMesh->getNormalsSize());
			offset += sizeof(Vector3)*subMesh->getNormalsSize();
		}

		if(tangents)
		{
			render->setVBOSubData(VBO_ARRAY, offset, tangents, sizeof(Vector3)*subMesh->getTangentsSize());
			offset += sizeof(Vector3)*subMesh->getTangentsSize();
		}

		if(texCoords)
		{
			render->setVBOSubData(VBO_ARRAY, offset, texCoords, sizeof(Vector2)*subMesh->getTexCoordsSize());
			offset += sizeof(Vector2)*subMesh->getTexCoordsSize();
		}

		if(colors)
		{
			render->setVBOSubData(VBO_ARRAY, offset, colors, sizeof(Color)*subMesh->getColorsSize());
			offset += sizeof(Color)*subMesh->getColorsSize();
		}

		// indices VBO
		if(indices)
		{
			unsigned int typeSize = indicesType == VAR_USHORT ? sizeof(short) : sizeof(int);

			if (*vboId2 == 0)
				render->createVBO(vboId2);
			
			render->bindVBO(VBO_ELEMENT_ARRAY, *vboId2);
			render->setVBO(VBO_ELEMENT_ARRAY, indices, subMesh->getIndicesSize()*typeSize, mode);
		}

		render->bindVBO(VBO_ARRAY, 0);
		render->bindVBO(VBO_ELEMENT_ARRAY, 0);
	}
}

void StandardRenderer::set2D(unsigned int w, unsigned int h)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	render->setViewport(0, 0, w, h);

	// set ortho projection
	render->setMatrixMode(MATRIX_PROJECTION);
	render->loadIdentity();

	render->setOrthoView(0.0f, (float)w, (float)h, 0.0f, 1.0f, -1.0f);

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();
}

void StandardRenderer::drawQuad(int fx)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

	// screen size
	int screenWidth = 0;
	int screenHeight = 0;
	NeoEngine::getInstance()->getSystemContext()->getScreenSize((unsigned int*) &screenWidth, (unsigned int*) &screenHeight);

	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;

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
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

void StandardRenderer::initQuadVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	if(*vao || *vbo || *texcoordVbo)
		clearQuadVAO(vao, vbo, texcoordVbo);

	render->createVAO(vao);
	render->bindVAO(*vao);

	render->createVBO(vbo);
	render->bindVBO(VBO_ARRAY, *vbo);
	render->setVBO(VBO_ARRAY, m_vertices, 4*sizeof(Vector2), VBO_STATIC);

	render->createVBO(texcoordVbo);
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->setVBO(VBO_ARRAY, m_texCoords, 4*sizeof(Vector2), VBO_STATIC);

	// Send Vertex data
	int vertexAttrib;
	int texcoordAttrib;

	// Vertex
	render->bindVBO(VBO_ARRAY, *vbo);
	render->getAttribLocation(fx, "Vertex", &vertexAttrib);
	render->enableAttribArray(vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

	// TexCoord
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	render->enableAttribArray(texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

	render->bindVAO(0);
}
void StandardRenderer::initTextVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	// Update vertex cache
	Vector2 vertices[4];
	vertices[0] = Vector2(0, 0);
	vertices[1] = Vector2(0, 1);
	vertices[3] = Vector2(1, 1);
	vertices[2] = Vector2(1, 0);

	render->createVAO(vao);
	render->bindVAO(*vao);

	render->createVBO(vbo);
	render->bindVBO(VBO_ARRAY, *vbo);
	render->setVBO(VBO_ARRAY, vertices, 4 * sizeof(Vector2), VBO_DYNAMIC);

	render->createVBO(texcoordVbo);
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->setVBO(VBO_ARRAY, m_texCoords, 4 * sizeof(Vector2), VBO_DYNAMIC);

	// Send Vertex data
	int vertexAttrib;
	int texcoordAttrib;

	// Vertex
	render->bindVBO(VBO_ARRAY, *vbo);
	render->getAttribLocation(fx, "Vertex", &vertexAttrib);
	render->enableAttribArray(vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

	// TexCoord
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	render->enableAttribArray(texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

	render->bindVAO(0);
}


void StandardRenderer::clearQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	render->deleteVAO(vao);
	render->deleteVBO(vbo);
	render->deleteVBO(texcoordVbo);
}


void StandardRenderer::stopThreads()
{
	m_lightUpdateSemaphore->WaitAndLock();
	m_visibilityUpdateSemaphore->WaitAndLock();
}

void StandardRenderer::startThreads()
{
	m_lightUpdateSemaphore->Unlock();
	m_visibilityUpdateSemaphore->Unlock();
}

////////////////////////////////////////////////////////////////////////////////
//
// UPDATE THREADS
//
////////////////////////////////////////////////////////////////////////////////

#define THREAD_SLEEP 50

int StandardRenderer::light_update_thread(void* data)
{
   StandardRenderer* self = (StandardRenderer*) data;
   if(!self)
	   return 1;

   NeoEngine* engine = NeoEngine::getInstance();
   NeoWindow* window = NeoWindow::getInstance();

   Scene* scene;
   OLight* light;

   while (!engine->getGame()->isRunning()) window->sleep(100);

   while(engine->isActive())
   {
	   if (!engine->getGame()->isRunning())
		   continue;

	   StandardRenderer* render = (StandardRenderer*) engine->getRenderer();
	   render->m_lightUpdateSemaphore->WaitAndLock();
	   Level* level = engine->getLevel();

	   // Aquire lock
	   for(int i = 0; i < level->getScenesNumber(); i++)
	   {
		   scene = level->getSceneByIndex(i);
		   OCamera* camera;

		   if(!scene || !(camera = scene->getCurrentCamera()))
			   break;

		   CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());

		   if(!data)
		   {
			   data = new CameraData();
			   camera->setAdditionalData(data);
		   }

		   data->lightLock->WaitAndLock();
		   data->visibleLights.clear();

		   // Fill the light buffer with all visible lights
		   int j = 0;
		   for(int i = 0; i < scene->getLightsNumber() && i < MAX_ENTITY_LIGHTS; i++)
		   {
			   	// If light is visible
			   	if ((light = scene->getLightByIndex(i)) != NULL && light->isActive() && (light->isVisible() || light->getSpotAngle() == 0))
			   	{
					data->visibleLights.push_back(light);
					j++;
				}
		   }

		   data->lightLock->Unlock();
	   }

	   render->m_lightUpdateSemaphore->Unlock();
	   window->sleep(THREAD_SLEEP);
   }

   return 0;
}

/*
 * Scene update threads
 */

/// UUUUGLY!
Vector3 g_referenceCameraPos;
bool zCompare(const OEntity* lhs, const OEntity* rhs)
{
	return (lhs->getTransformedPosition() - g_referenceCameraPos).getLength() > (rhs->getTransformedPosition() - g_referenceCameraPos).getLength();
}

int StandardRenderer::visibility_thread_mainscene(void* data)
{
	NeoEngine* engine = NeoEngine::getInstance();
	NeoWindow* window = NeoWindow::getInstance();

	// Wait for engine to start the game
	while (!engine->getGame()->isRunning()) window->sleep(100);

	// Initialize scenes with additional data
	/*Level* level = engine->getLevel();
	for (int i = 0; i < level->getScenesNumber(); i++)
	{
		Scene* s = level->getSceneByIndex(i);
		s->setAdditionalData(new SceneData());

		for (int j = 0; j < s->getCamerasNumber(); j++)
		{
			s->getCameraByIndex(j)->setAdditionalData(new CameraData());
		}
	}*/

	while (engine->isActive())
	{
		if (!engine->getGame()->isRunning())
			continue;

		StandardRenderer* render = (StandardRenderer*) engine->getRenderer();
		render->m_visibilityUpdateSemaphore->WaitAndLock();

		// Preparation
		Level* level = engine->getLevel();
		Scene* scene;
		OCamera* camera;
		
		for(int i = 0; i < level->getScenesNumber(); i++)
		{
			scene = level->getSceneByIndex(i);
			if(!scene || !(camera = scene->getCurrentCamera()))
			{
				window->sleep(THREAD_SLEEP);
				continue;
			}

			CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());

			if(!data)
			{
				data = new CameraData();
				camera->setAdditionalData(data);

				//window->sleep(THREAD_SLEEP);
				//continue;
			}

			camera->enable();
			camera->getFrustum()->makeVolume(camera);

			// Culling!
			data->visibilityLock->WaitAndLock();
			data->visibleEntities.clear();
			data->visibleTransparentEntities.clear();

			size_t sz = scene->getEntitiesNumber();
			for (int i = 0; i < sz; i++)
			{
				OEntity *e = scene->getEntityByIndex(i);

				if (e->isActive()) {
					e->updateVisibility(camera);

					if (e->isVisible()) {
						if (e->hasTransparency())
							data->visibleTransparentEntities.push_back(e);
						else
							data->visibleEntities.push_back(e);
						//MLOG_INFO(e->getName() << " is visible!");
					}
				}
			}

			g_referenceCameraPos = camera->getTransformedPosition();
			std::sort(data->visibleTransparentEntities.begin(), data->visibleTransparentEntities.end(), zCompare);

			//while(1);
			data->visibilityLock->Unlock();
		}


		render->m_visibilityUpdateSemaphore->Unlock();
		// MLOG_INFO("Got " << data->visibleEntities.size() << " entities! ");
		window->sleep(THREAD_SLEEP);

	}

	return 0;
}

#endif // MOBILE_RENDERER

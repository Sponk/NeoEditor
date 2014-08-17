/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MSkybox.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <www.scary-squid.de>
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

#include <MEngine.h>
#include <string>

#define SKYBOX_SIZE 1000

using namespace std;

string skyboxVertShader =

"#version 140\n"
"attribute vec3 Vertex;"
"uniform mat4 ProjModelViewMatrix;"

"varying vec2 texCoord;"
"attribute vec2 TexCoord;"

"void main(void)"
"{"
    "gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
    "texCoord = TexCoord;"
"}\n";

string skyboxFragShader =
"#version 140\n"
"uniform sampler2D Texture[5];"
"varying vec2 texCoord;"

"void main(void)"
"{"
    "gl_FragColor = texture2D(Texture[0], texCoord);"
"}\n";

MSkybox::MSkybox() :
    m_init(false)
{
    for(int i = 0; i < 6; i++)
    {
        m_SkyboxTexture[i] = NULL;
    }
}

void MSkybox::drawSkybox(MVector3 position, MVector3 rotation)
{
    MRenderingContext* render = MEngine::getInstance()->getRenderingContext();
    if(!m_init)
    {
        return;
    }

    render->bindFX(m_fx);
    static MVector2 texCoords[4];

    texCoords[0] = MVector2(1, 0);
    texCoords[1] = MVector2(0, 0);
    texCoords[3] = MVector2(0, 1);
    texCoords[2] = MVector2(1, 1);

    // TODO: Cache coords!
    // unten 1
    render->bindTexture(m_SkyboxTexture[1]->getTextureId());
    drawQuad(MVector3(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE), MVector3(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
                MVector3(SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), MVector3(-SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), position, -rotation, (MVector2*) &texCoords);

    texCoords[0] = MVector2(1, 1);
    texCoords[1] = MVector2(0, 1);
    texCoords[3] = MVector2(0, 0);
    texCoords[2] = MVector2(1, 0);

    // oben 2
    render->bindTexture(m_SkyboxTexture[4]->getTextureId());
    drawQuad(MVector3(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE), MVector3(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE),
                MVector3(SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), MVector3(-SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), position, -rotation, (MVector2*) &texCoords);

    texCoords[0] = MVector2(0, 1);
    texCoords[1] = MVector2(1, 1);
    texCoords[3] = MVector2(1, 0);
    texCoords[2] = MVector2(0, 0);

    // vorne 3
    render->bindTexture(m_SkyboxTexture[5]->getTextureId());
    drawQuad(MVector3(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE), MVector3(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
            MVector3(-SKYBOX_SIZE,SKYBOX_SIZE, SKYBOX_SIZE), MVector3(SKYBOX_SIZE,SKYBOX_SIZE, SKYBOX_SIZE), position, -rotation, (MVector2*) &texCoords);

    texCoords[0] = MVector2(1, 1);
    texCoords[1] = MVector2(0, 1);
    texCoords[3] = MVector2(0, 0);
    texCoords[2] = MVector2(1, 0);

    // hinten 4
    render->bindTexture(m_SkyboxTexture[2]->getTextureId());
    drawQuad(MVector3(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE), MVector3(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE),
               MVector3(-SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), MVector3(SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), position, -rotation, (MVector2*) &texCoords);

    texCoords[0] = MVector2(0, 0);
    texCoords[1] = MVector2(0, 1);
    texCoords[3] = MVector2(1, 1);
    texCoords[2] = MVector2(1, 0);

    // links 5
    render->bindTexture(m_SkyboxTexture[3]->getTextureId());
    drawQuad(MVector3(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE), MVector3(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
                MVector3(-SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), MVector3(-SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), position, -rotation, (MVector2*) &texCoords);


    texCoords[3] = MVector2(0, 1);
    texCoords[2] = MVector2(0, 0);
    texCoords[1] = MVector2(1, 1);
    texCoords[0] = MVector2(1, 0);

    // rechts 6
    render->bindTexture(m_SkyboxTexture[0]->getTextureId());
    drawQuad(MVector3(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE), MVector3(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
                MVector3(SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), MVector3(SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), position, -rotation, (MVector2*) &texCoords);

    render->bindFX(0);
}

void MSkybox::loadSkyboxTextures(const char *path)
{
    MRenderingContext* render = MEngine::getInstance()->getRenderingContext();
    MEngine* engine = MEngine::getInstance();

    char globalFilename[256];
    getGlobalFilename(globalFilename, engine->getSystemContext()->getWorkingDirectory(), path);

    if(!strcmp(globalFilename, m_path.getSafeString()))
        return;

    m_path.set(globalFilename);

    if(!m_init)
    {
        render->createVertexShader(&m_vertShad);
        render->sendShaderSource(m_vertShad, skyboxVertShader.c_str());

        render->createPixelShader(&m_pixShad);
        render->sendShaderSource(m_pixShad, skyboxFragShader.c_str());

        render->createFX(&m_fx, m_vertShad, m_pixShad);
    }

    if(m_fx == 0)
    {
        MLOG_ERROR("Could not initialize skybox shader! Skybox will be disabled!");
        return;
    }

    for(int i = 0; i < 6; i++)
    {
        if(m_SkyboxTexture[i])
            SAFE_DELETE(m_SkyboxTexture[i]);
    }

    string basePath = globalFilename;
    const char* names[] = {"negx", "negy", "negz", "posx", "posy", "posz"};
    MLevel* level = engine->getLevel();

    for(int i = 0; i < 6; i++)
    {
        if((m_SkyboxTexture[i] = level->loadTexture((basePath + "/" + names[i] + ".jpg").c_str())) == NULL)
        {
            MLOG_ERROR("Could not load texture \"" << basePath + "/" + names[i] + ".jpg\"");
            m_init = false;
            return;
        }
    }

    m_init = true;
}

void MSkybox::drawQuad(MVector3 v1, MVector3 v2, MVector3 v3, MVector3 v4, MVector3 position, MVector3 rotation, MVector2* texCoords)
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

    int vertexAttrib;
    int texcoordAttrib;
    static MVector3 vertices[4];

    vertices[0] = v1;
    vertices[1] = v2;
    vertices[3] = v3;
    vertices[2] = v4;

    // projmodelview matrix
    static MMatrix4x4 ProjMatrix;
    static MMatrix4x4 ModelViewMatrix;
    static MMatrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);

    ModelViewMatrix.loadIdentity();
    ModelViewMatrix.translate(position);

    if(rotation.x > 0)
        rotation.x *= -1;
    // printf("rotation(%f, %f, %f)\n", rotation.x, rotation.y, rotation.z);

    // First, rotate X and Y so Z points up
    ModelViewMatrix.setRotationEuler(rotation.x,rotation.y, 0);

    // Rotate around the Z axis
    ModelViewMatrix.rotate(MVector3(0,0,1), rotation.z);

    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
    render->sendUniformMatrix(m_fx, "ProjModelViewMatrix", &ProjModelViewMatrix);

    // Texture
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(m_fx, "Textures", texIds, 4);

    // Vertex
    render->getAttribLocation(m_fx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, M_FLOAT, 3, vertices);
    render->enableAttribArray(vertexAttrib);

    // TexCoord
    render->getAttribLocation(m_fx, "TexCoord", &texcoordAttrib);
    render->setAttribPointer(texcoordAttrib, M_FLOAT, 2, texCoords);
    render->enableAttribArray(texcoordAttrib);

    // draw
    render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

    render->disableAttribArray(vertexAttrib);
    render->disableAttribArray(texcoordAttrib);
}

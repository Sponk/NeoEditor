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

#include <NeoEngine.h>
#include <string>

using namespace Neo;

#define SKYBOX_SIZE 100

using namespace std;

string m_skyboxVertShader =

"#version 130\n"
"attribute vec3 Vertex;"
"uniform mat4 ProjModelViewMatrix;"

"varying vec2 texCoord;"
"attribute vec2 TexCoord;"

"void main(void)"
"{"
    "gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
    "texCoord = TexCoord;"
"}\n";

string m_skyboxFragShader =
"#version 130\n"
"uniform sampler2D Texture[5];"
"varying vec2 texCoord;"

"void main(void)"
"{"
    "gl_FragColor = texture2D(Texture[0], texCoord);"
"}\n";

Skybox::Skybox() :
    m_init(false)
{
    for(int i = 0; i < 6; i++)
    {
        m_SkyboxTexture[i] = NULL;
    }
}

void Skybox::drawSkybox(Vector3 position, Vector3 rotation)
{
    RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
    if(!m_init)
    {
        return;
    }

    // projmodelview matrix
    rotation *= -1;
    static Matrix4x4 ProjMatrix;
    static Matrix4x4 ModelViewMatrix;
    static Matrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);

    ModelViewMatrix.loadIdentity();

    ModelViewMatrix.rotate(Vector3(1.0,0.0,0.0), rotation.x);
    ModelViewMatrix.rotate(Vector3(0.0,1.0,0.0), rotation.y);
    ModelViewMatrix.rotate(Vector3(0.0,0.0,1.0), rotation.z);

    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;

    render->disableDepthTest();
    render->bindFX(m_fx);
    static Vector2 texCoords[4];

    texCoords[0] = Vector2(1, 0);
    texCoords[1] = Vector2(0, 0);
    texCoords[3] = Vector2(0, 1);
    texCoords[2] = Vector2(1, 1);

    // TODO: Cache coords!
    // unten 1
    render->bindTexture(m_SkyboxTexture[1]->getTextureId());
    drawQuad(Vector3(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE), Vector3(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
                Vector3(SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), Vector3(-SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), &ProjModelViewMatrix, (Vector2*) &texCoords);

    texCoords[0] = Vector2(1, 1);
    texCoords[1] = Vector2(0, 1);
    texCoords[3] = Vector2(0, 0);
    texCoords[2] = Vector2(1, 0);

    // oben 2
    render->bindTexture(m_SkyboxTexture[4]->getTextureId());
    drawQuad(Vector3(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE), Vector3(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE),
                Vector3(SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), Vector3(-SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), &ProjModelViewMatrix, (Vector2*) &texCoords);

    texCoords[0] = Vector2(0, 1);
    texCoords[1] = Vector2(1, 1);
    texCoords[3] = Vector2(1, 0);
    texCoords[2] = Vector2(0, 0);

    // vorne 3
    render->bindTexture(m_SkyboxTexture[5]->getTextureId());
    drawQuad(Vector3(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE), Vector3(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
            Vector3(-SKYBOX_SIZE,SKYBOX_SIZE, SKYBOX_SIZE), Vector3(SKYBOX_SIZE,SKYBOX_SIZE, SKYBOX_SIZE), &ProjModelViewMatrix, (Vector2*) &texCoords);

    texCoords[0] = Vector2(1, 1);
    texCoords[1] = Vector2(0, 1);
    texCoords[3] = Vector2(0, 0);
    texCoords[2] = Vector2(1, 0);

    // hinten 4
    render->bindTexture(m_SkyboxTexture[2]->getTextureId());
    drawQuad(Vector3(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE), Vector3(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE),
               Vector3(-SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), Vector3(SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), &ProjModelViewMatrix, (Vector2*) &texCoords);

    texCoords[0] = Vector2(0, 0);
    texCoords[1] = Vector2(0, 1);
    texCoords[3] = Vector2(1, 1);
    texCoords[2] = Vector2(1, 0);

    // links 5
    render->bindTexture(m_SkyboxTexture[3]->getTextureId());
    drawQuad(Vector3(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE), Vector3(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
                Vector3(-SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), Vector3(-SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), &ProjModelViewMatrix, (Vector2*) &texCoords);


    texCoords[3] = Vector2(0, 1);
    texCoords[2] = Vector2(0, 0);
    texCoords[1] = Vector2(1, 1);
    texCoords[0] = Vector2(1, 0);

    // rechts 6
    render->bindTexture(m_SkyboxTexture[0]->getTextureId());
    drawQuad(Vector3(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE), Vector3(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE),
                Vector3(SKYBOX_SIZE,-SKYBOX_SIZE, -SKYBOX_SIZE), Vector3(SKYBOX_SIZE,-SKYBOX_SIZE, SKYBOX_SIZE), &ProjModelViewMatrix, (Vector2*) &texCoords);

    render->bindFX(0);
}

void Skybox::loadSkyboxTextures(const char *path)
{
    if(strlen(path) == 0)
    {
        m_init = false;
        m_path.clear();
        return;
    }

    RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
    NeoEngine* engine = NeoEngine::getInstance();

    char globalFilename[256];
    getGlobalFilename(globalFilename, engine->getSystemContext()->getWorkingDirectory(), path);

    if(!strcmp(globalFilename, m_path.getSafeString()))
        return;

    m_path.set(path);

    if(!m_init)
    {
        render->createVertexShader(&m_vertShad);
        bool success = render->sendShaderSource(m_vertShad, m_skyboxVertShader.c_str());

        render->createPixelShader(&m_pixShad);
        success &= render->sendShaderSource(m_pixShad, m_skyboxFragShader.c_str());

        if(!success)
        {
            m_fx = 0;
        }
        else
        {
            render->createFX(&m_fx, m_vertShad, m_pixShad);
        }
    }

    if(m_fx == 0)
    {
        MLOG_ERROR("Could not initialize skybox shader! Skybox will be disabled!");
        return;
    }

    string basePath = globalFilename;
    const char* names[] = {"negx", "negy", "negz", "posx", "posy", "posz"};
    Level* level = engine->getLevel();

    for(int i = 0; i < 6; i++)
    {
        if((m_SkyboxTexture[i] = level->loadTexture((basePath + "/" + names[i] + ".jpg").c_str())) == NULL)
        {
            MLOG_ERROR("Could not load texture \"" << basePath + "/" + names[i] + ".jpg\"");
            m_path.clear();
            m_init = false;
            return;
        }

        render->bindTexture(m_SkyboxTexture[i]->getTextureId());
        render->setTextureUWrapMode(WRAP_CLAMP);
        render->setTextureVWrapMode(WRAP_CLAMP);
    }

    render->bindTexture(0);
    m_init = true;
}

void Skybox::drawQuad(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Matrix4x4* matrix, Vector2* texCoords)
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    int vertexAttrib;
    int texcoordAttrib;
    static Vector3 vertices[4];

    vertices[0] = v1;
    vertices[1] = v2;
    vertices[3] = v3;
    vertices[2] = v4;

    render->sendUniformMatrix(m_fx, "ProjModelViewMatrix", matrix);

    // Texture
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(m_fx, "Textures", texIds, 4);

    // Vertex
    render->getAttribLocation(m_fx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, VAR_FLOAT, 3, vertices);
    render->enableAttribArray(vertexAttrib);

    // TexCoord
    render->getAttribLocation(m_fx, "TexCoord", &texcoordAttrib);
    render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, texCoords);
    render->enableAttribArray(texcoordAttrib);

    // draw
    render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);

    render->disableAttribArray(vertexAttrib);
    render->disableAttribArray(texcoordAttrib);
}

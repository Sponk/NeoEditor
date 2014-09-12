//
// Copyright (C) 2014
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <MEngine.h>
#include <cmath>
#include <string>
#include <algorithm>

using std::string;

#define MULT 1

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

void MPostProcessor::eraseTextures()
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

    if(m_ColourTexID != 0)
        render->deleteTexture(&m_ColourTexID);

    if(m_DepthTexID != 0)
        render->deleteTexture(&m_DepthTexID);

    if(m_BufferID != 0)
        render->deleteFrameBuffer(&m_BufferID);
}

void MPostProcessor::updateResolution()
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
    MSystemContext * system = MEngine::getInstance()->getSystemContext();

    if(system == NULL || render == NULL)
        return;

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    m_Resolution = Pow2(max(screenWidth, screenHeight));

    m_Resolution *= MULT;

    // create frame buffer
    render->createFrameBuffer(&m_BufferID);

    // create render textures
    render->createTexture(&m_ColourTexID);
    render->bindTexture(m_ColourTexID);
    render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(M_WRAP_CLAMP);
    render->setTextureVWrapMode(M_WRAP_CLAMP);
    render->texImage(0, m_Resolution, m_Resolution, M_UBYTE, M_RGB, 0);

    render->createTexture(&m_DepthTexID);
    render->bindTexture(m_DepthTexID);
    render->setTextureFilterMode(M_TEX_FILTER_NEAREST, M_TEX_FILTER_NEAREST);
    render->texImage(0, m_Resolution, m_Resolution, M_UBYTE, M_DEPTH, 0);
}

MPostProcessor::MPostProcessor()
    : m_BufferID(0)
    , m_ColourTexID(0)
    , m_DepthTexID(0)
    , m_fx(0)
    , m_fragShad(0)
    , m_vertShad(0)
{
    updateResolution();
}

bool MPostProcessor::draw(MOCamera* camera)
{
    if(m_fx == 0)
        return false;

    MEngine * engine = MEngine::getInstance(); // get the engine instance
    MRenderingContext * render = engine->getRenderingContext(); // get the rendering context
    MSystemContext * system = engine->getSystemContext();

    unsigned int currentFrameBuffer = 0;
    render->getCurrentFrameBuffer(&currentFrameBuffer);

    if(strcmp(engine->getRenderer()->getName(), "FixedRenderer") == 0)
        return false;

    // get level
    MLevel * level = MEngine::getInstance()->getLevel();
    if(! level)
        return false;

    // get current scene
    MScene * scene = level->getCurrentScene();
    if(! scene)
        return false;

    camera->enable();
    camera->updateListener();

    MVector3 clearColor = camera->getClearColor();
    render->setClearColor(clearColor);

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // render to texture
    render->bindFrameBuffer(m_BufferID);
    render->attachFrameBufferTexture(M_ATTACH_COLOR0, m_ColourTexID);
    render->attachFrameBufferTexture(M_ATTACH_DEPTH, m_DepthTexID);
    render->setViewport(0, 0, m_Resolution, m_Resolution); // change viewport

    render->setClearColor(camera->getClearColor());

    if(!camera->getSkybox()->isInitialized())
        render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);
    else
        render->clear(M_BUFFER_DEPTH);

    render->enableDepthTest();

    scene->draw(camera);
    scene->drawObjectsBehaviors();

    // finish render to texture
    render->bindFrameBuffer(currentFrameBuffer);

    // draw the rendered textured with a shader effect
    render->setViewport(0, 0, screenWidth, screenHeight);
    render->setClearColor(MVector3(1, 0, 0));
    render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

    set2D(screenWidth, screenHeight);

    render->bindFX(m_fx);
    render->bindTexture(m_ColourTexID);
    render->bindTexture(m_DepthTexID, 1);

    drawQuad(MVector2((float)screenWidth, (float)screenHeight));
    render->bindFX(0);
    return true;
}

bool MPostProcessor::loadShader(const char* vertShad, const char* fragShad)
{
    if(vertShad == NULL || fragShad == NULL)
        return false;

    eraseTextures();
    updateResolution();

    MRenderingContext * render = MEngine::getInstance()->getRenderingContext(); // get the rendering context
    bool success = false;

    if(m_vertShad)
        render->deleteShader(&m_vertShad);

    if(m_fragShad)
        render->deleteShader(&m_fragShad);

    render->createVertexShader(&m_vertShad);
    success = render->sendShaderSource(m_vertShad, vertShad);

    render->createPixelShader(&m_fragShad);
    success = success && render->sendShaderSource(m_fragShad, fragShad);

    if(success)
        render->createFX(&m_fx, m_vertShad, m_fragShad);
    else
    {
        m_fx = 0;
        MLOG_ERROR("Could not apply post effects shader!");
    }

    return success;
}

void MPostProcessor::set2D(unsigned int w, unsigned int h)
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
    render->setViewport(0, 0, w, h);

    // set ortho projection
    render->setMatrixMode(M_MATRIX_PROJECTION);
    render->loadIdentity();

    render->setOrthoView(0.0f, (float)w, (float)h, 0.0f, 1.0f, -1.0f);

    render->setMatrixMode(M_MATRIX_MODELVIEW);
    render->loadIdentity();
}

void MPostProcessor::drawQuad(MVector2 scale)
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

    int vertexAttrib;
    int texcoordAttrib;
    static MVector2 vertices[4];
    static MVector2 texCoords[4];

    vertices[0] = MVector2(0, 0);
    vertices[1] = MVector2(0, scale.y);
    vertices[3] = MVector2(scale.x, scale.y);
    vertices[2] = MVector2(scale.x, 0);

    texCoords[0] = MVector2(0, 1);
    texCoords[1] = MVector2(0, 0);
    texCoords[3] = MVector2(1, 0);
    texCoords[2] = MVector2(1, 1);

    // Send settings to shader
    sendUniforms();

    // projmodelview matrix
    static MMatrix4x4 ProjMatrix;
    static MMatrix4x4 ModelViewMatrix;
    static MMatrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);
    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
    render->sendUniformMatrix(m_fx, "ProjModelViewMatrix", &ProjModelViewMatrix);

    // Texture
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(m_fx, "Textures", texIds, 4);

    // Vertex
    render->getAttribLocation(m_fx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, M_FLOAT, 2, vertices);
    render->enableAttribArray(vertexAttrib);

    // TexCoord
    render->getAttribLocation(m_fx, "TexCoord", &texcoordAttrib);
    render->setAttribPointer(texcoordAttrib, M_FLOAT, 2, texCoords);
    render->enableAttribArray(texcoordAttrib);

    // Width
    render->sendUniformFloat(m_fx, "Width", &scale.x, 1);
    // Height
    render->sendUniformFloat(m_fx, "Height", &scale.y, 1);

    // draw
    render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

    render->disableAttribArray(vertexAttrib);
    render->disableAttribArray(texcoordAttrib);
}

void MPostProcessor::addFloatUniform(const char* name)
{
    float_uniform_t uniform;
    strcpy(uniform.name, name);

    uniform.value = 0;
    uniform.dirty = true;

    m_FloatUniformList.push_back(uniform);
}

void MPostProcessor::addIntUniform(const char* name)
{
    int_uniform_t uniform;
    strcpy(uniform.name, name);

    uniform.value = 0;
    uniform.dirty = true;

    m_IntUniformList.push_back(uniform);
}

void MPostProcessor::setIntUniformValue(const char* name, int value)
{
    for(unsigned int i = 0; i < m_IntUniformList.size(); i++)
    {
        if(!strcmp(m_IntUniformList[i].name, name))
        {
            m_IntUniformList[i].value = value;
            return;
        }
    }
}

void MPostProcessor::setFloatUniformValue(const char* name, float value)
{
    for(unsigned int i = 0; i < m_FloatUniformList.size(); i++)
    {
        if(!strcmp(m_FloatUniformList[i].name, name))
        {
            m_FloatUniformList[i].value = value;
            m_FloatUniformList[i].dirty = true;
            return;
        }
    }
}

void MPostProcessor::sendUniforms()
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

    // Set dynamic uniforms
    float_uniform_t float_uniform;

    for(unsigned int i = 0; i < m_FloatUniformList.size(); i++)
    {
        float_uniform = m_FloatUniformList[i];

        if(float_uniform.dirty)
        {
            render->sendUniformFloat(m_fx, float_uniform.name, &float_uniform.value);
            m_FloatUniformList[i].dirty = false;
        }
    }

    int_uniform_t int_uniform;

    for(unsigned int i = 0; i < m_IntUniformList.size(); i++)
    {
        int_uniform = m_IntUniformList[i];

        if(int_uniform.dirty)
        {
            render->sendUniformInt(m_fx, int_uniform.name, &int_uniform.value);
            m_FloatUniformList[i].dirty = false;
        }
    }
}

void MPostProcessor::clear()
{
    m_FloatUniformList.clear();
    m_IntUniformList.clear();
}

//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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
#include <cmath>
#include <string>
#include <algorithm>

using namespace Neo;
using std::string;

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

void PostProcessor::eraseTextures()
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    if(m_ColourTexID != 0)
        render->deleteTexture(&m_ColourTexID);

    if(m_DepthTexID != 0)
        render->deleteTexture(&m_DepthTexID);

    if(m_BufferID != 0)
        render->deleteFrameBuffer(&m_BufferID);
}

void PostProcessor::updateResolution()
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
    SystemContext * system = NeoEngine::getInstance()->getSystemContext();

    if(system == NULL || render == NULL)
        return;

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // Update vertex cache
    m_vertices[0] = Vector2(0, 0);
    m_vertices[1] = Vector2(0, screenHeight);
    m_vertices[3] = Vector2(screenWidth, screenHeight);
    m_vertices[2] = Vector2(screenWidth, 0);

    m_Resolution = Pow2(max(screenWidth, screenHeight));

    if(m_ResolutionMultiplier == 0) m_ResolutionMultiplier = 1.0f;

    m_Resolution *= m_ResolutionMultiplier;

    // create frame buffer
    render->createFrameBuffer(&m_BufferID);

    // create render textures
    render->createTexture(&m_ColourTexID);
    render->bindTexture(m_ColourTexID);
    render->setTextureFilterMode(TEX_FILTER_LINEAR, TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(WRAP_CLAMP);
    render->setTextureVWrapMode(WRAP_CLAMP);
    render->texImage(0, m_Resolution, m_Resolution, VAR_FLOAT, TEX_RGB, 0);

    render->createTexture(&m_DepthTexID);
    render->bindTexture(m_DepthTexID);
    render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
    render->texImage(0, m_Resolution, m_Resolution, VAR_UBYTE, TEX_DEPTH, 0);
}

PostProcessor::PostProcessor()
    : m_BufferID(0)
    , m_ColourTexID(0)
    , m_DepthTexID(0)
    , m_fx(0)
    , m_fragShad(0)
    , m_vertShad(0)
    , m_ResolutionMultiplier(1.0f)
{
    updateResolution();

    m_texCoords[0] = Vector2(0, 1);
    m_texCoords[1] = Vector2(0, 0);
    m_texCoords[3] = Vector2(1, 0);
    m_texCoords[2] = Vector2(1, 1);
}

bool PostProcessor::draw(OCamera* camera)
{
    if(m_fx == 0)
        return false;

    NeoEngine * engine = NeoEngine::getInstance(); // get the engine instance
    RenderingContext * render = engine->getRenderingContext(); // get the rendering context
    SystemContext * system = engine->getSystemContext();

    unsigned int currentFrameBuffer = 0;
    render->getCurrentFrameBuffer(&currentFrameBuffer);

    if(strcmp(engine->getRenderer()->getName(), "FixedRenderer") == 0)
        return false;

    // get level
    Level * level = NeoEngine::getInstance()->getLevel();
    if(! level)
        return false;

    // get current scene
    Scene * scene = level->getCurrentScene();
    if(! scene)
        return false;

    camera->enable();
    camera->updateListener();

    Vector3 clearColor = camera->getClearColor();
    render->setClearColor(clearColor);

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // render to texture
    render->bindFrameBuffer(m_BufferID);
    render->attachFrameBufferTexture(ATTACH_COLOR0, m_ColourTexID);
    render->attachFrameBufferTexture(ATTACH_DEPTH, m_DepthTexID);
    render->setViewport(0, 0, m_Resolution, m_Resolution); // change viewport

    render->setClearColor(camera->getClearColor());

    if(!camera->getSkybox()->isInitialized())
        render->clear(BUFFER_COLOR | BUFFER_DEPTH);
    else
        render->clear(BUFFER_DEPTH);

    render->enableDepthTest();
    render->setScissor(0,0,m_Resolution,m_Resolution);
    render->disableScissorTest();

    scene->draw(camera);
    scene->drawObjectsBehaviors();

    // finish render to texture
    render->bindFrameBuffer(currentFrameBuffer);

    // draw the rendered textured with a shader effect
    render->setViewport(0, 0, screenWidth, screenHeight);
    render->setClearColor(Vector3(1, 0, 0));
    render->clear(BUFFER_COLOR | BUFFER_DEPTH);

    set2D(screenWidth, screenHeight);

    render->bindFX(m_fx);
    render->bindTexture(m_ColourTexID);
    render->bindTexture(m_DepthTexID, 1);
    render->disableBlending();

    drawQuad(Vector2((float)screenWidth, (float)screenHeight));
    render->bindFX(0);
    return true;
}

bool PostProcessor::loadShader(const char* vertShad, const char* fragShad)
{
    if(vertShad == NULL || fragShad == NULL)
        return false;

    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext(); // get the rendering context
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

bool PostProcessor::loadShaderFile(const char* vertShad, const char* fragShad)
{
    char path[255];
    getGlobalFilename(path, NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(), vertShad);
    char* vert = readTextFile(path);

    getGlobalFilename(path, NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(), fragShad);
    char* frag = readTextFile(path);

    bool ret = loadShader(vert, frag);

    if(ret)
    {
        char path[255];
        getLocalFilename(path, NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(), vertShad);
        m_vertShadPath.set(path);

        getLocalFilename(path, NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(), fragShad);
        m_fragShadPath.set(path);
    }

    SAFE_FREE(vert);
    SAFE_FREE(frag);

    return ret;
}

void PostProcessor::set2D(unsigned int w, unsigned int h)
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

void PostProcessor::drawQuad(Vector2 scale)
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    int vertexAttrib;
    int texcoordAttrib;

    // Send settings to shader
    sendUniforms();

    // projmodelview matrix
    static Matrix4x4 ProjMatrix;
    static Matrix4x4 ModelViewMatrix;
    static Matrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);
    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
    render->sendUniformMatrix(m_fx, "ProjModelViewMatrix", &ProjModelViewMatrix);

    // Texture
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(m_fx, "Textures", texIds, 4);

    // Vertex
    render->getAttribLocation(m_fx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, m_vertices);
    render->enableAttribArray(vertexAttrib);

    // TexCoord
    render->getAttribLocation(m_fx, "TexCoord", &texcoordAttrib);
    render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, m_texCoords);
    render->enableAttribArray(texcoordAttrib);

    // Width
    render->sendUniformFloat(m_fx, "Width", &scale.x, 1);
    // Height
    render->sendUniformFloat(m_fx, "Height", &scale.y, 1);

    // draw
    render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);

    render->disableAttribArray(vertexAttrib);
    render->disableAttribArray(texcoordAttrib);
}

void PostProcessor::addFloatUniform(const char* name)
{
    float_uniform_t* uniform = (float_uniform_t*) malloc(sizeof(float_uniform_t));
    uniform->variable = NeoVariable(uniform->name, (void*) &uniform->value, M_VARIABLE_FLOAT);

    strcpy(uniform->name, name);
    uniform->value = 0;
    uniform->dirty = true;

    m_UniformList.push_back((uintptr_t) uniform);
}

float PostProcessor::getFloatUniformValue(int idx)
{
    if(idx > m_UniformList.size() || idx < 0)
        return 0.0;

    uniform_t* uniform = (uniform_t*) m_UniformList[idx];
    if(uniform && uniform->variable.getType() == M_VARIABLE_FLOAT)
        return *((float*)uniform->variable.getPointer());

    return 0.0;
}

void PostProcessor::setFloatUniformValue(const char* name, float value)
{
    for(unsigned int i = 0; i < m_UniformList.size(); i++)
    {
        if(!strcmp(((uniform_t*)m_UniformList[i])->name, name) && ((uniform_t*)m_UniformList[i])->variable.getType() == M_VARIABLE_FLOAT)
        {
            ((float_uniform_t*)m_UniformList[i])->value = value;
            ((float_uniform_t*) m_UniformList[i])->dirty = true;
            return;
        }
    }
}

void PostProcessor::addIntUniform(const char* name)
{
    int_uniform_t* uniform = (int_uniform_t*) malloc(sizeof(int_uniform_t));
    uniform->variable = NeoVariable(uniform->name, (void*) &uniform->value, M_VARIABLE_INT);

    strcpy(uniform->name, name);
    uniform->value = 0;
    uniform->dirty = true;

    m_UniformList.push_back((uintptr_t) uniform);
}

int PostProcessor::getIntUniformValue(int idx)
{
    if(idx > m_UniformList.size() || idx < 0)
        return 0;

    uniform_t* uniform = (uniform_t*) m_UniformList[idx];
    if(uniform && uniform->variable.getType() == M_VARIABLE_INT)
        return *((int*)uniform->variable.getPointer());

    return 0;
}

void PostProcessor::setIntUniformValue(const char* name, float value)
{
    for(unsigned int i = 0; i < m_UniformList.size(); i++)
    {
        if(!strcmp(((uniform_t*)m_UniformList[i])->name, name) && ((uniform_t*)m_UniformList[i])->variable.getType() == M_VARIABLE_INT)
        {
            ((int_uniform_t*)m_UniformList[i])->value = value;
            ((int_uniform_t*) m_UniformList[i])->dirty = true;
            return;
        }
    }
}

const char* PostProcessor::getUniformName(int idx)
{
    if(idx > m_UniformList.size() -1 || idx < 0)
        return NULL;

    uniform_t* uniform = (uniform_t*) m_UniformList[idx];
    if(uniform)
        return uniform->variable.getName();

    return NULL;
}

M_VARIABLE_TYPE PostProcessor::getUniformType(int idx)
{
    if(idx > m_UniformList.size() -1 || idx < 0)
        return M_VARIABLE_NULL;

    uniform_t* uniform = (uniform_t*) m_UniformList[idx];
    if(uniform)
        return uniform->variable.getType();

    return M_VARIABLE_NULL;
}

void PostProcessor::sendUniforms()
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // Set dynamic uniforms
    uniform_t* uniform;

    for(unsigned int i = 0; i < m_UniformList.size(); i++)
    {
        uniform = ((uniform_t*)m_UniformList[i]);
        if(!uniform->dirty)
            continue;

        switch(uniform->variable.getType())
        {
            case M_VARIABLE_FLOAT:
            {
               render->sendUniformFloat(m_fx, uniform->name, (float*) uniform->variable.getPointer());
               uniform->dirty = false;
            }
            break;

            case M_VARIABLE_INT:
            {
               render->sendUniformInt(m_fx, uniform->name, (int*) uniform->variable.getPointer());
               uniform->dirty = false;
            }
            break;
        }
    }
}

void PostProcessor::clear()
{
    for(int i = 0; i < m_UniformList.size(); i++)
    {
        free((void*) m_UniformList[i]);
    }

    m_UniformList.clear();
}

void PostProcessor::setShaderPath(const char* vertPath, const char* fragPath)
{
    char path[255];
    getLocalFilename(path, NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(), vertPath);
    m_vertShadPath.set(path);

    getLocalFilename(path, NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(), fragPath);
    m_fragShadPath.set(path);
}

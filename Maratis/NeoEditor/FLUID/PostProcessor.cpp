#include "PostProcessor.h"
#include "../Maratis/Maratis.h"

bool PostProcessor::draw(MOCamera *camera)
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

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // render to texture
    render->bindFrameBuffer(m_BufferID);
    render->attachFrameBufferTexture(M_ATTACH_COLOR0, m_ColourTexID);
    render->attachFrameBufferTexture(M_ATTACH_DEPTH, m_DepthTexID);
    render->setViewport(0, 0, m_textureWidth, m_textureHeight); // change viewport

    render->enableDepthTest();

    render->setScissor(0,0,m_textureWidth, m_textureHeight);
    render->disableScissorTest();

    render->clear(M_BUFFER_DEPTH | M_BUFFER_COLOR);

    Maratis::getInstance()->drawMainView(MEngine::getInstance()->getLevel()->getCurrentScene());

    // finish render to texture
    render->bindFrameBuffer(currentFrameBuffer);

    // draw the rendered textured with a shader effect
    render->setViewport(0, 0, screenWidth, screenHeight);
    render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

    set2D(screenWidth, screenHeight);

    render->bindFX(m_fx);
    render->bindTexture(m_ColourTexID);
    render->bindTexture(m_DepthTexID, 1);
    render->disableBlending();

    drawQuad(MVector2((float)screenWidth, (float)screenHeight));
    render->bindFX(0);
    return true;
}

void PostProcessor::updateResolution()
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
    MSystemContext * system = MEngine::getInstance()->getSystemContext();

    if(system == NULL || render == NULL)
        return;

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // Update vertex cache
    m_vertices[0] = MVector2(0, 0);
    m_vertices[1] = MVector2(0, screenHeight);
    m_vertices[3] = MVector2(screenWidth, screenHeight);
    m_vertices[2] = MVector2(screenWidth, 0);

    m_textureWidth = screenWidth;
    m_textureHeight = screenHeight;

    // create frame buffer
    render->createFrameBuffer(&m_BufferID);

    // create render textures
    render->createTexture(&m_ColourTexID);
    render->bindTexture(m_ColourTexID);
    render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(M_WRAP_CLAMP);
    render->setTextureVWrapMode(M_WRAP_CLAMP);
    render->texImage(0, m_textureWidth, m_textureHeight, M_FLOAT, M_RGB, 0);

    render->createTexture(&m_DepthTexID);
    render->bindTexture(m_DepthTexID);
    render->setTextureFilterMode(M_TEX_FILTER_NEAREST, M_TEX_FILTER_NEAREST);
    render->texImage(0, m_textureWidth, m_textureHeight, M_UBYTE, M_DEPTH, 0);
}

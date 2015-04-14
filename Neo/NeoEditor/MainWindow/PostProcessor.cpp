//========================================================================
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

#include "../MainWindow/PostProcessor.h"

#include "../Backend/EditorBackend.h"

using namespace Neo;

bool EditorPostProcessor::draw(OCamera *camera)
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

    // screen size
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    system->getScreenSize(&screenWidth, &screenHeight);

    // render to texture
    render->bindFrameBuffer(m_BufferID);
    render->attachFrameBufferTexture(ATTACH_COLOR0, m_ColourTexID);
    render->attachFrameBufferTexture(ATTACH_DEPTH, m_DepthTexID);
    render->setViewport(0, 0, m_textureWidth, m_textureHeight); // change viewport

    render->enableDepthTest();

    render->setScissor(0,0,m_textureWidth, m_textureHeight);
    render->disableScissorTest();

    render->clear(BUFFER_DEPTH | BUFFER_COLOR);

    EditorBackend::getInstance()->drawMainView(NeoEngine::getInstance()->getLevel()->getCurrentScene());

    // finish render to texture
    render->bindFrameBuffer(currentFrameBuffer);

    // draw the rendered textured with a shader effect
    render->setViewport(0, 0, screenWidth, screenHeight);
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

void EditorPostProcessor::updateResolution()
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

    m_textureWidth = screenWidth;
    m_textureHeight = screenHeight;

    // create frame buffer
    render->createFrameBuffer(&m_BufferID);

    // create render textures
    render->createTexture(&m_ColourTexID);
    render->bindTexture(m_ColourTexID);
    render->setTextureFilterMode(TEX_FILTER_LINEAR, TEX_FILTER_LINEAR);
    render->setTextureUWrapMode(WRAP_CLAMP);
    render->setTextureVWrapMode(WRAP_CLAMP);
    render->texImage(0, m_textureWidth, m_textureHeight, VAR_FLOAT, TEX_RGB, 0);

    render->createTexture(&m_DepthTexID);
    render->bindTexture(m_DepthTexID);
    render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
    render->texImage(0, m_textureWidth, m_textureHeight, VAR_UBYTE, TEX_DEPTH, 0);
}

/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of NeoGui.
 *
 * NeoGui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NeoGui is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with NeoGui.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von NeoGui.
 *
 * NeoGui ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * NeoGui wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <Render.h>
#include <MEngine.h>
#include <GuiSystem.h>

using namespace Neo::Gui;

const char* m_colorOnlyVertShader =

"#version 130\n"
"attribute vec3 Vertex;"
"uniform mat4 ProjModelViewMatrix;"
"void main(void)"
"{"
    "gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
"}\n";

const char* m_colorOnlyFragShader =
"#version 130\n"
"uniform vec4 color;"
"void main(void)"
"{"
    "gl_FragColor = color;"
"}\n";

const char* m_texturedVertShader =

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

const char* m_texturedFragShader =
"#version 130\n"
"uniform sampler2D Texture[5];"
"varying vec2 texCoord;"

"void main(void)"
"{"
    "gl_FragColor = texture2D(Texture[0], texCoord);"
"}\n";


Render::Render() :
    m_colorOnlyFx(0),
    m_texturedFx(0)
{

}

void Render::drawColoredQuad(float x, float y, float w, float h, MVector4 color)
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

    // Don't render anything if there is nothing to render
    if(color.z == 0)
        return;

    if(m_colorOnlyFx == 0)
    {
        loadShader(m_colorOnlyVertShader, m_colorOnlyFragShader, &m_colorOnlyFx);
    }

    int vertexAttrib;
    render->pushMatrix();

    MVector2 m_vertices[4];
    m_vertices[0] = MVector2(x, y);
    m_vertices[1] = MVector2(x, y+h);
    m_vertices[3] = MVector2(x+w, y+h);
    m_vertices[2] = MVector2(x+w, y);

    // Set up env
    render->bindFX(m_colorOnlyFx);
    render->enableBlending();
    render->disableCullFace();

    // projmodelview matrix
    static MMatrix4x4 ProjMatrix;
    static MMatrix4x4 ModelViewMatrix;
    static MMatrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);
    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;

    render->sendUniformMatrix(m_colorOnlyFx, "ProjModelViewMatrix", &ProjModelViewMatrix);

    // Vertex
    render->getAttribLocation(m_colorOnlyFx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, M_FLOAT, 2, m_vertices);
    render->enableAttribArray(vertexAttrib);

    // Width
    render->sendUniformFloat(m_colorOnlyFx, "Width", &w, 1);
    // Height
    render->sendUniformFloat(m_colorOnlyFx, "Height", &h, 1);

    // Color
    render->sendUniformVec4(m_colorOnlyFx, "color", color);

    // draw
    render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

    render->disableAttribArray(vertexAttrib);
    render->bindFX(0);
    render->disableBlending();
    render->enableCullFace();

    render->popMatrix();
}

void Render::drawTexturedQuad(float x, float y, float w, float h, int texture)
{
    MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

    if(m_texturedFx == 0)
    {
        loadShader(m_texturedVertShader, m_texturedFragShader, &m_texturedFx);
    }

    int vertexAttrib;
    int texcoordAttrib;

    render->pushMatrix();

    MVector2 m_vertices[4];
    m_vertices[0] = MVector2(x, y);
    m_vertices[1] = MVector2(x, y+h);
    m_vertices[3] = MVector2(x+w, y+h);
    m_vertices[2] = MVector2(x+w, y);

    MVector2 m_texcoords[4];
    m_texcoords[0] = MVector2(0, 0);
    m_texcoords[1] = MVector2(0, 1);
    m_texcoords[2] = MVector2(1, 0);
    m_texcoords[3] = MVector2(1, 1);

    // Set up env
    render->bindFX(m_texturedFx);
    render->enableBlending();
    render->enableTexture();
    render->bindTexture(texture);

    // projmodelview matrix
    static MMatrix4x4 ProjMatrix;
    static MMatrix4x4 ModelViewMatrix;
    static MMatrix4x4 ProjModelViewMatrix;

    render->getProjectionMatrix(&ProjMatrix);
    render->getModelViewMatrix(&ModelViewMatrix);
    ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;

    render->sendUniformMatrix(m_texturedFx, "ProjModelViewMatrix", &ProjModelViewMatrix);

    // Vertex
    render->getAttribLocation(m_texturedFx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, M_FLOAT, 2, m_vertices);
    render->enableAttribArray(vertexAttrib);

    // Texcoords
    // TexCoord
    render->getAttribLocation(m_texturedFx, "TexCoord", &texcoordAttrib);
    render->setAttribPointer(texcoordAttrib, M_FLOAT, 2, m_texcoords);
    render->enableAttribArray(texcoordAttrib);

    // Width
    render->sendUniformFloat(m_texturedFx, "Width", &w, 1);
    // Height
    render->sendUniformFloat(m_texturedFx, "Height", &h, 1);

    // draw
    render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

    render->disableAttribArray(vertexAttrib);
    render->disableAttribArray(texcoordAttrib);

    render->bindFX(0);
    render->disableBlending();

    render->popMatrix();
}


void Render::set2D(float w, float h)
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

void Render::loadShader(const char* vert, const char* frag, unsigned int* fx)
{
    MRenderingContext* render = MEngine::getInstance()->getRenderingContext();

    unsigned int m_vertShad, m_pixShad;
    render->createVertexShader(&m_vertShad);
    bool success = render->sendShaderSource(m_vertShad, vert);

    render->createPixelShader(&m_pixShad);
    success &= render->sendShaderSource(m_pixShad, frag);

    if(!success)
    {
        *fx = 0;
    }
    else
    {
        render->createFX(fx, m_vertShad, m_pixShad);
    }
}

void Render::drawText(MOText* text, float x, float y)
{
    MRenderingContext* renderContext = MEngine::getInstance()->getRenderingContext();

    text->setPosition(MVector3(x, y, 0));
    text->updateMatrix();

    renderContext->pushMatrix();
    renderContext->multMatrix(text->getMatrix());

    renderContext->enableTexture();

    MEngine::getInstance()->getRenderer()->drawText(text);
    renderContext->disableTexture();
    renderContext->popMatrix();
}

MOText* Render::createText(const char* font, float size)
{
    if(!font)
        return NULL;

    MSystemContext* system = MEngine::getInstance()->getSystemContext();

    char file[256];
    getGlobalFilename(file, system->getWorkingDirectory(), font);

    MOText* text;
    text = new MOText(MEngine::getInstance()->getLevel()->loadFont(file));
    text->setSize(size);

    return text;
}

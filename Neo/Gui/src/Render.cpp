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
#include <NeoEngine.h>
#include <GuiSystem.h>

using namespace Neo::Gui;
using namespace Neo;

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

void Render::drawColoredQuad(float x, float y, float w, float h, MVector4 color, float rotation)
{
    MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // Don't render anything if there is nothing to render
    if(color.z == 0)
        return;

    if(m_colorOnlyFx == 0)
    {
        loadShader(m_colorOnlyVertShader, m_colorOnlyFragShader, &m_colorOnlyFx);
    }

    int vertexAttrib;
    //render->pushMatrix();

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

    MVector3 pivot = MVector3(x+0.5*w, y+0.5*h, 0);
    ModelViewMatrix.translate(pivot);
    ModelViewMatrix.rotate(MVector3(0,0,1), rotation);
    ModelViewMatrix.translate(-pivot);

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

    //render->popMatrix();
}



void Render::drawTexturedQuad(float x, float y, float w, float h, int texture, float rotation)
{
	drawTexturedQuad(x, y, w, h, texture, rotation, MVector2(1, 1), MVector2(1, 1), MVector4(0.0, 0.0, 1.0, 1.0));
}
void Render::drawTexturedQuad(float x, float y, float w, float h, int texture, float rotation, MVector2 scale)
{
	drawTexturedQuad(x, y, w, h, texture, rotation, scale, MVector2(1, 1), MVector4(0.0, 0.0, 1.0, 1.0));
}
void Render::drawTexturedQuad(float x, float y, float w, float h, int texture, float rotation, MVector2 scale, MVector2 flip)
{
	drawTexturedQuad(x, y, w, h, texture, rotation, scale, flip, MVector4(0.0, 0.0, 1.0, 1.0));
}

void Render::drawTexturedQuad(float x, float y, float w, float h, int texture, float rotation, MVector2 scale, MVector2 flip, MVector4 texcoords)
{
	MRenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

	if (m_texturedFx == 0)
	{
		loadShader(m_texturedVertShader, m_texturedFragShader, &m_texturedFx);
	}

	int vertexAttrib;
	int texcoordAttrib;

	//render->pushMatrix();

	MVector2 m_vertices[4];
	m_vertices[0] = MVector2(x, y);
	m_vertices[1] = MVector2(x, y + h);
	m_vertices[2] = MVector2(x + w, y);
	m_vertices[3] = MVector2(x + w, y + h);
	
	MVector2 m_texcoords[4];
	/*m_texcoords[0] = MVector2(0, 0);
	m_texcoords[1] = MVector2(0, 1);
	m_texcoords[2] = MVector2(1, 0);
	m_texcoords[3] = MVector2(1, 1);*/
	if (flip.x != 0 || flip.y != 0){
		if (flip.x > 0)
		{
			// normal way around
			left = texcoords.x;
			right = texcoords.x + (texcoords.z * flip.x);
		}
		else
		{
			right = texcoords.x;
			left = texcoords.x + (texcoords.z * -flip.x);
		}

		if (flip.y > 0)
		{
			top = texcoords.y;
			bottom = texcoords.y + (texcoords.w * flip.y);
		}
		else
		{
			bottom = texcoords.y;
			top = texcoords.y + (texcoords.w * -flip.y);
		}
		m_texcoords[0] = MVector2(left, top);
		m_texcoords[1] = MVector2(left, bottom);
		m_texcoords[2] = MVector2(right, top);
		m_texcoords[3] = MVector2(right, bottom);

	}
	else
	{
		m_texcoords[0] = MVector2(texcoords.x, texcoords.y);
		m_texcoords[1] = MVector2(texcoords.x, texcoords.y + texcoords.w);
		m_texcoords[2] = MVector2(texcoords.x + texcoords.z, texcoords.y);
		m_texcoords[3] = MVector2(texcoords.x + texcoords.z, texcoords.y + texcoords.w);
	}
	

	// Set up env
	render->bindFX(m_texturedFx);
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);
	render->bindTexture(texture);

	// projmodelview matrix
	static MMatrix4x4 ProjMatrix;
	static MMatrix4x4 ModelViewMatrix;
	static MMatrix4x4 ProjModelViewMatrix;

	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);

	MVector3 pivot = MVector3(x + 0.5*w, y + 0.5*h, 0);
	ModelViewMatrix.translate(pivot);
	ModelViewMatrix.rotate(MVector3(0, 0, 1), rotation);
	ModelViewMatrix.translate(-pivot);

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
	render->bindTexture(0);
	render->disableBlending();

	//render->popMatrix();
}




void Render::set2D(float w, float h)
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

void Render::loadShader(const char* vert, const char* frag, unsigned int* fx)
{
    MRenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

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

void Render::drawText(OText* text, float x, float y, float rotation)
{
    MRenderingContext* renderContext = NeoEngine::getInstance()->getRenderingContext();

    text->setPosition(MVector3(x, y, 0));
	text->setRotation(MQuaternion(0,0,rotation));
    text->updateMatrix();

    renderContext->pushMatrix();
    renderContext->multMatrix(text->getMatrix());

    renderContext->enableTexture();

    NeoEngine::getInstance()->getRenderer()->drawText(text);
    renderContext->disableTexture();
    renderContext->popMatrix();
}

OText* Render::createText(const char* font, float size)
{
    if(!font)
        return NULL;

    MSystemContext* system = NeoEngine::getInstance()->getSystemContext();

    char file[256];
    getGlobalFilename(file, system->getWorkingDirectory(), font);

    OText* text;
    text = new OText(NeoEngine::getInstance()->getLevel()->loadFont(file));
    text->setSize(size);

    return text;
}

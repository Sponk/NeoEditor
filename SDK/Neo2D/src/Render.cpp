/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of Neo2D.
 *
 * Neo2D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Neo2D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Neo2D.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von Neo2D.
 *
 * Neo2D ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * Neo2D wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit
 *diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <Render.h>
#include <NeoEngine.h>
#include <Neo2DEngine.h>

using namespace Neo;
using namespace Neo2D;

const char* m_colorOnlyVertShader =

	"#version 130\n"
	"attribute vec3 Vertex;"
	"uniform mat4 ProjModelViewMatrix;"
	"void main(void)"
	"{"
	"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
	"}\n";

const char* m_colorOnlyFragShader = "#version 130\n"
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

Render::Render() : m_colorOnlyFx(0), m_texturedFx(0), m_colorVao(0), m_textureVao(0) {}

void Render::init(unsigned int fx, unsigned int* vao)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	unsigned int vbo, texcoordVbo;

	Vector2 texCoords[4];
	texCoords[0] = Vector2(0, 1);
	texCoords[1] = Vector2(0, 0);
	texCoords[3] = Vector2(1, 0);
	texCoords[2] = Vector2(1, 1);

	Vector2 vertices[4];
	vertices[0] = Vector2(0, 0);
	vertices[1] = Vector2(0, 1);
	vertices[3] = Vector2(1, 1);
	vertices[2] = Vector2(1, 0);

	render->createVAO(vao);
	render->bindVAO(*vao);

	render->createVBO(&vbo);
	render->bindVBO(VBO_ARRAY, vbo);
	render->setVBO(VBO_ARRAY, vertices, 4 * sizeof(Vector2), VBO_STATIC);

	render->createVBO(&texcoordVbo);
	render->bindVBO(VBO_ARRAY, texcoordVbo);
	render->setVBO(VBO_ARRAY, texCoords, 4 * sizeof(Vector2), VBO_STATIC);

	// Send Vertex data
	int vertexAttrib;
	int texcoordAttrib;

	// Vertex
	render->bindVBO(VBO_ARRAY, vbo);
	render->getAttribLocation(fx, "Vertex", &vertexAttrib);
	render->enableAttribArray(vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

	// TexCoord
	render->bindVBO(VBO_ARRAY, texcoordVbo);
	render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	render->enableAttribArray(texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

	render->bindVAO(0);
}

void Render::drawColoredQuad(float x, float y, float w, float h, Vector4 color,
							 float rotation)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();
	
	// Don't render anything if there is nothing to render
	if (color.z == 0)
		return;

	if (m_colorOnlyFx == 0)
	{
		loadShader(m_colorOnlyVertShader, m_colorOnlyFragShader,
				   &m_colorOnlyFx);

		init(m_colorOnlyFx, &m_colorVao);
	}

	int vertexAttrib;
	// render->pushMatrix();

	/*Vector2 m_vertices[4];
	m_vertices[0] = Vector2(x, y);
	m_vertices[1] = Vector2(x, y + h);
	m_vertices[3] = Vector2(x + w, y + h);
	m_vertices[2] = Vector2(x + w, y);*/

	// Set up env
	render->bindVAO(m_colorVao);
	render->bindFX(m_colorOnlyFx);
	render->enableBlending();
	render->disableCullFace();
	render->disableDepthTest();

	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;

	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);
	
	ModelViewMatrix.setScale(Vector3(w, h, 1));
	ProjMatrix.translate(Vector3(x, y, 0));

	Vector3 pivot = Vector3(x + 0.5 * w, y + 0.5 * h, 0);
	ModelViewMatrix.translate(pivot);
	ModelViewMatrix.rotate(Vector3(0, 0, 1), rotation);
	ModelViewMatrix.translate(-pivot);
	
	ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;

	render->sendUniformMatrix(m_colorOnlyFx, "ProjModelViewMatrix",
							  &ProjModelViewMatrix);

	// Vertex
	/*render->getAttribLocation(m_colorOnlyFx, "Vertex", &vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, m_vertices);
	render->enableAttribArray(vertexAttrib);*/

	// Width
	render->sendUniformFloat(m_colorOnlyFx, "Width", &w, 1);
	// Height
	render->sendUniformFloat(m_colorOnlyFx, "Height", &h, 1);

	// Color
	render->sendUniformVec4(m_colorOnlyFx, "color", color);

	// draw
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);

	//render->disableAttribArray(vertexAttrib);
	render->bindFX(0);
	render->disableBlending();
	render->enableCullFace();

	render->bindVAO(0);
	// render->popMatrix();
}

void Render::drawTexturedQuad(float x, float y, float w, float h, int texture,
							  float rotation)
{
	drawTexturedQuad(x, y, w, h, texture, rotation, Vector2(1, 1),
					 Vector2(1, 1), Vector4(0.0, 0.0, 1.0, 1.0));
}
void Render::drawTexturedQuad(float x, float y, float w, float h, int texture,
							  float rotation, Vector2 scale)
{
	drawTexturedQuad(x, y, w, h, texture, rotation, scale, Vector2(1, 1),
					 Vector4(0.0, 0.0, 1.0, 1.0));
}
void Render::drawTexturedQuad(float x, float y, float w, float h, int texture,
							  float rotation, Vector2 scale, Vector2 flip)
{
	drawTexturedQuad(x, y, w, h, texture, rotation, scale, flip,
					 Vector4(0.0, 0.0, 1.0, 1.0));
}
void Render::drawTexturedQuad(float x, float y, float w, float h, int texture,
							  float rotation, Vector2 scale, Vector2 flip,
							  Vector4 texcoords)
{

	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	if (m_texturedFx == 0)
	{
		loadShader(m_texturedVertShader, m_texturedFragShader, &m_texturedFx);
		init(m_texturedFx, &m_textureVao);
	}

	int vertexAttrib;
	int texcoordAttrib;
	Vector2 m_vertices[4];

	/*m_vertices[0] = Vector2(x, y);
	m_vertices[1] = Vector2(x, y + h);
	m_vertices[2] = Vector2(x + w, y);
	m_vertices[3] = Vector2(x + w, y + h);

	Vector2 m_texcoords[4];
	m_texcoords[0] = Vector2(texcoords.x, texcoords.y);
	m_texcoords[1] = Vector2(texcoords.x, texcoords.y + texcoords.w);
	m_texcoords[2] = Vector2(texcoords.x + texcoords.z, texcoords.y);
	m_texcoords[3] = Vector2(texcoords.x + texcoords.z, texcoords.y + texcoords.w);

	if (flip.x != 0 || flip.y != 0)
	{
		if (flip.x > 0)
		{
			m_left = x;
			m_right = x + (w * flip.x);
		}
		else
		{
			m_right = x;
			m_left = x + (w * -flip.x);
		}
		if (flip.y > 0)
		{
			m_top = y;
			m_bottom = y + (h * flip.y);
		}
		else
		{
			m_bottom = y;
			m_top = y + (h * -flip.y);
		}

		m_vertices[0] = Vector2(m_left, m_top);
		m_vertices[1] = Vector2(m_left, m_bottom);
		m_vertices[2] = Vector2(m_right, m_top);
		m_vertices[3] = Vector2(m_right, m_bottom);
	}*/

	// Set up env
	render->bindVAO(m_textureVao);
	render->bindFX(m_texturedFx);
	render->enableBlending();
	render->setBlendingMode(BLENDING_ALPHA);
	render->disableDepthTest();
	render->bindTexture(texture);

	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;
	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);

	ModelViewMatrix.setScale(Vector3(w, h, 1));
	
	Vector3 pivot = Vector3(x + 0.5 * w, y + 0.5 * h, 0.0);

	ProjMatrix.translate(pivot);

	ProjMatrix.rotate(Vector3(1, 0, 0), flip.x);
	ProjMatrix.rotate(Vector3(0, 1, 0), flip.y);
	ProjMatrix.rotate(Vector3(0, 0, 1), rotation);
	ProjMatrix.translate(-pivot);

	ProjMatrix.translate(Vector3(x, y, 0));

	//ModelViewMatrix.translate(pivot);
	//ModelViewMatrix.rotate(Vector3(0, 0, 1), rotation);
	//ModelViewMatrix.rotate(Vector3(0, 1, 0), 180);
	//ModelViewMatrix.rotate(Vector3(1, 0, 0), rotation);

	//ModelViewMatrix.translate(-pivot);
	ModelViewMatrix.scale(Vector3(scale.x, scale.y, 0));

	ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
	render->sendUniformMatrix(m_texturedFx, "ProjModelViewMatrix",
							  &ProjModelViewMatrix);
	// Vertex
	/*render->getAttribLocation(m_texturedFx, "Vertex", &vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, m_vertices);
	render->enableAttribArray(vertexAttrib);*/

	// Texcoords
	/*render->getAttribLocation(m_texturedFx, "TexCoord", &texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, m_texcoords);
	render->enableAttribArray(texcoordAttrib);*/

	// Width
	render->sendUniformFloat(m_texturedFx, "Width", &w, 1);
	// Height
	render->sendUniformFloat(m_texturedFx, "Height", &h, 1);

	// draw
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);
	//render->disableAttribArray(vertexAttrib);
	//render->disableAttribArray(texcoordAttrib);
	render->bindFX(0);
	render->bindTexture(0);
	render->disableBlending();

	render->bindVAO(0);
}

void Render::set2D(float w, float h)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	render->setViewport(0, 0, w, h);

	// set ortho projection
	render->setMatrixMode(MATRIX_PROJECTION);
	render->loadIdentity();

	render->setOrthoView(0.0f, (float)w, (float)h, 0.0f, 1.0f, -1.0f);

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();

	m_resolution = Vector2(w, h);
}

void Render::loadShader(const char* vert, const char* frag, unsigned int* fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	unsigned int m_vertShad, m_pixShad;
	render->createVertexShader(&m_vertShad);
	bool success = render->sendShaderSource(m_vertShad, vert);

	render->createPixelShader(&m_pixShad);
	success &= render->sendShaderSource(m_pixShad, frag);

	if (!success)
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
	RenderingContext* renderContext =
		NeoEngine::getInstance()->getRenderingContext();

	text->setPosition(Vector3(x, y, 0));
	text->setRotation(Quaternion(0, 0, rotation));
	text->updateMatrix();

	renderContext->pushMatrix();
	renderContext->multMatrix(text->getMatrix());

	renderContext->enableTexture();
	renderContext->disableDepthTest();

	NeoEngine::getInstance()->getRenderer()->drawText(text);
	renderContext->disableTexture();
	renderContext->popMatrix();
}

OText* Render::createText(const char* font, float size)
{
	if (!font)
		return NULL;

	SystemContext* system = NeoEngine::getInstance()->getSystemContext();

	char file[256];
	getGlobalFilename(file, system->getWorkingDirectory(), font);

	OText* text;
	//FontRef* fontref = FontRef::getNew(NULL, file);
	FontRef* fontref = NeoEngine::getInstance()->getLevel()->loadFont(file, size); 
	
	fontref->getFont()->setFontSize(size);
	fontref->update();

	text = new OText(fontref);
	text->setSize(fontref->getFont()->getFontSize());

	return text;
}

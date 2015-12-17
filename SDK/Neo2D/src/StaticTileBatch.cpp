/*
 * Copyright 2015 (C) Yannick Pflanzer <neo-engine.de>
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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <Render.h>
#include <StaticTileBatch.h>
#include <NeoEngine.h>

/// FIXME: Use the shader from Render.cpp!!
static const char* m_texturedVertShader =

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

static const char* m_texturedFragShader =
	"#version 130\n"
		"uniform sampler2D Texture[5];"
		"varying vec2 texCoord;"

		"void main(void)"
		"{"
		"gl_FragColor = texture2D(Texture[0], texCoord);"
		"}\n";

using namespace Neo2D;
using namespace Neo;

StaticTileBatch::~StaticTileBatch()
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	if(m_vao)
		render->deleteVAO(&m_vao);

	if(m_texcoordVbo)
		render->deleteVBO(&m_texcoordVbo);

	if(m_vertexVbo)
		render->deleteVBO(&m_vertexVbo);

	if(m_fx)
		render->deleteFX(&m_fx);
}

void StaticTileBatch::addTile(float x, float y, float scale, unsigned int tx, unsigned int ty)
{
	//Vector3 pivot = Vector3(x + 0.5 * w, y + 0.5 * h, 0.0);
	Vector2 origin(x, y);
	m_vertices.push_back(origin);
	m_vertices.push_back(origin + Vector2(0, m_tilesheet->getTileHeight()) * scale);
	m_vertices.push_back(origin + Vector2(m_tilesheet->getTileWidth(), m_tilesheet->getTileHeight()) * scale);
	m_vertices.push_back(origin + Vector2(m_tilesheet->getTileWidth(), 0) * scale);

	Vector4 tilecoords = m_tilesheet->getTexCoords(tx, ty);
	m_texcoords.push_back(Vector2(tilecoords.x, tilecoords.y + tilecoords.w));
	m_texcoords.push_back(Vector2(tilecoords.x, tilecoords.y));
	m_texcoords.push_back(Vector2(tilecoords.x + tilecoords.z, tilecoords.y + tilecoords.w));
	m_texcoords.push_back(Vector2(tilecoords.x + tilecoords.z, tilecoords.y));
}

void StaticTileBatch::draw(Vector2 offset)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	// Build VAO and VBOs
	if(!m_vao)
	{
		render->createVAO(&m_vao);
		render->bindVAO(m_vao);

		render->createVBO(&m_vertexVbo);
		render->bindVBO(VBO_ARRAY, m_vertexVbo);
		render->setVBO(VBO_ARRAY, m_vertices.data(), m_vertices.size() * sizeof(Vector2), VBO_STATIC);

		render->createVBO(&m_texcoordVbo);
		render->bindVBO(VBO_ARRAY, m_texcoordVbo);
		render->setVBO(VBO_ARRAY, m_texcoords.data(), m_texcoords.size() * sizeof(Vector2), VBO_STATIC);

		// Send Vertex data
		int vertexAttrib;
		int texcoordAttrib;

		Render::getInstance()->loadShader(m_texturedVertShader, m_texturedFragShader, &m_fx);

		// Vertex
		render->bindVBO(VBO_ARRAY, m_vertexVbo);
		render->getAttribLocation(m_fx, "Vertex", &vertexAttrib);
		render->enableAttribArray(vertexAttrib);
		render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

		// TexCoord
		render->bindVBO(VBO_ARRAY, m_texcoordVbo);
		render->getAttribLocation(m_fx, "TexCoord", &texcoordAttrib);
		render->enableAttribArray(texcoordAttrib);
		render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

		render->bindVAO(0);
	}

	// Set up env
	render->bindVAO(m_vao);
	render->bindFX(m_fx);

	render->enableBlending();
	render->setBlendingMode(BLENDING_ALPHA);

	render->disableDepthTest();
	render->disableCullFace();
	render->bindTexture(m_tilesheet->getImage());

	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;
	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);

	ModelViewMatrix.setTranslation(Vector3(offset.x, offset.y, 0));
	ModelViewMatrix.setScale(Vector3(1, 1, 1));

	ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
	render->sendUniformMatrix(m_fx, "ProjModelViewMatrix",
							  &ProjModelViewMatrix);

	// draw
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, m_vertices.size());

	render->bindFX(0);
	render->bindTexture(0);
	render->disableBlending();

	render->bindVAO(0);
}


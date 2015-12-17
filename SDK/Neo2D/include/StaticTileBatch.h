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

#ifndef __STATIC_TILE_BATCH_H
#define __STATIC_TILE_BATCH_H

#include <NeoCore.h>
#include <Canvas.h>
#include <Tile.h>
#include <vector>

namespace Neo2D
{
class NEO2D_EXPORT StaticTileBatch : public SpriteBatch
{
	TileSheet* m_tilesheet;
	std::vector<Vector2> m_vertices;
	std::vector<Vector2> m_texcoords;

	unsigned int m_vao;
	unsigned int m_texcoordVbo;
	unsigned int m_vertexVbo;
	unsigned int m_fx;

public:
	StaticTileBatch(TileSheet* tilesheet) :
		m_tilesheet(tilesheet),
		m_vao(0),
		m_texcoordVbo(0),
		m_vertexVbo(0)
	{}

	~StaticTileBatch();

	void addTile(float x, float y, float scale, unsigned int tx, unsigned int ty);
	virtual void draw(Vector2 offset);
};
}

#endif

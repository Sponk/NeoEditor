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

#ifndef __ANIMATED_TILE_H__
#define __ANIMATED_TILE_H__

#include <NeoEngine.h>
#include <Tile.h>
#include <vector>

namespace Neo2D
{
/**
 * @brief Contains primitives for basic frame based animations for use with sprite sheets.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Animation
{

private:
	unsigned int m_line;
	unsigned int m_start;
	unsigned int m_length;
	unsigned int m_delay;

	unsigned int m_current;
	unsigned int m_timer;

public:
	/**
	 * @brief Constructs a new animation.
	 *
	 * The animation starts at the coordinate (start, line) in the tile sheet and
	 * has a given length, so the last frame will be (start+length-1, line).
	 * Keep in mind that indices start at 0.
	 *
	 * @param start The first frame in the animation.
	 * @param length The length of the animations in number of frames.
	 * @param line The line in which the animations is located.
	 */
	Animation(unsigned int start, unsigned int line, unsigned int length, unsigned int delay)
		:
		m_start(start),
		m_length(length),
		m_delay(delay),
		m_line(line),
		m_current(start),
		m_timer(0)
	{ }

	Animation()
		: Animation(0, 0, 0, 0)
	{ }

	/**
	 * @brief Returns the line index in which the animations is found.
	 * @return The line index.
	 */
	unsigned int getLine() const { return m_line; }

	/**
	 * @brief Changes the line in which the animation is found.
	 * @param line The line.
	 */
	void setLine(unsigned int line) { m_line = line; }

	/**
	 * @brief Returns the start index in which the animations is found.
	 * @return The start index.
	 */
	unsigned int getStart() const {	return m_start;	}

	/**
	 * @brief Changes the first frame of the animation.
	 * @param start The first frame.
	 */
	void setStart(unsigned int start) {	this->m_start = start; }

	/**
	 * @brief Returns the length of the animation.
	 * @return The animations length.
	 */
	unsigned int getLength() const { return m_length; }

	/**
	 * @brief Changes the length of the animation.
	 * @param line The length.
	 */
	void setLength(unsigned int length) { m_length = length; }

	/**
	 * @brief Returns the delay in milliseconds.
	 * @return The delay between frames.
	 */
	unsigned int getDelay() const {	return m_delay; }

	/**
	 * @brief Changes the delay between frames in milliseconds.
	 * @param line The delay in milliseconds.
	 */
	void setDelay(unsigned int delay) {	m_delay = delay; }

	/**
	 * @brief Calculates the next frame in the animation.
	 *
	 * The animation always loops around when the last frame is over.
	 *
	 * @param dt The delta time in seconds.
	 * @return The next frame.
	 */
	unsigned int getNextFrame(float dt);
};

/**
 * @brief Implements a tile that automatically plays an animation.
 *
 * Each tile can have multiple animations attached and switch between them for different effects.
 * All animations need to be on the same tile sheet.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT AnimatedTile : public Tile
{
	std::vector<Animation> m_animations;
	size_t m_current;
public:

	AnimatedTile(float x, float y, unsigned int width,
		 unsigned int height, const char* label)
		: Tile(x, y, width, height, label, 0, 0), m_current(0)
	{}

	/**
	 * @brief Updates the tile and switches animations if requested.
	 */
	void update();

	/**
	 * @brief Adds a new animation to the list of animations.
	 *
	 * @param start The first frame in the animation.
	 * @param length The length of the animations in number of frames.
	 * @param line The line in which the animations is located.
	 * @return The new animation. Does not need to be deleted!
	 */
	Animation* addAnimation(unsigned int start, unsigned int line, unsigned int length, unsigned int delay);

	/**
	 * @brief Change current animation.
	 *
	 * Will change after the next call to update.
	 * @param idx The ID of the new animation.
	 */
	void setCurrentAnimation(size_t idx) { m_current = idx; }
};
}

#endif

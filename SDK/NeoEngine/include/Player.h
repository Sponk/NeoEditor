//========================================================================
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef __PLAYER_H
#define __PLAYER_H

#include <memory>

namespace Neo
{
/**
 * @brief Implements a basic player.
 *
 * Each player runs at a configurable speed. 
 * Additionally each instance handles a seperate set of loaded plugins.
 */
class NEO_ENGINE_EXPORT Player
{
	unsigned int m_frameCap;

#ifndef SWIG
	std::vector<std::shared_ptr<Plugin>> m_plugins;
#endif

	NeoEngine* m_engine;

 public:
	Player(unsigned int framecap) : m_frameCap(framecap)
	{
		// Cache current instance
		m_engine = NeoEngine::getInstance();
	}

	/**
	 * @brief Loads a plugin and registers it with the player object.
	 * @param name The name of the plugin to load.
	 * @return A boolean value representing success.
	 */
	bool loadPlugin(const char* name);

	/**
	 * @brief Starts the game.
	 * @param level The level to load.
	 */
	void begin(const char* level = "");

	/**
	 * @brief Executes a game loop which renders and updates the game.
	 *
	 * The update happens at a fixed rate and fills all remaining frame time
	 * by sleeping to ease the CPU load.
	 * The game loop will return as soon as the given key is being pressed
	 * or the game is not active anymore.
	 *
	 * @param quitKey OPTIONAL The game stops when this key is pressed.
	 */
	void execute(INPUT_KEYS quitKey = KEY_DUMMY);

	/**
	 * @brief Executes exactly one frame, i.e. one update and draw.
	 */
	void executeFrame();
	
	/**
	 * @brief Ends the game.
	 */
	void end();
};
}

#endif

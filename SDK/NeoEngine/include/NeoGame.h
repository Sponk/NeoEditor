//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 20014-2017 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef _NEO_GAME_H
#define _NEO_GAME_H

#ifndef DISABLE_PROFILING
#define PROFILE_BEGIN(name) { Neo::NeoEngine::getInstance()->getGame()->getProfiler().functionTimingStart(name); }
#define PROFILE_END(name) { Neo::NeoEngine::getInstance()->getGame()->getProfiler().functionTimingEnd(name); }
#else
#define PROFILE_BEGIN
#define PROFILE_END
#endif

#include <list>

namespace Neo
{

/**
 * This class represents a "sub-game" that contains overwritten
 * onBegin, update, draw and onEnd methods. This should be used by
 * plugins that do not want to replace the global NeoGame but just want to
 * install callbacks for those methods.
 */
class NEO_ENGINE_EXPORT SubGame
{
public:
	virtual void update(void) = 0;
	virtual void draw(void) = 0;

	virtual void onBegin(void) = 0;
	virtual void onEnd(void) = 0;
};

class NEO_ENGINE_EXPORT NeoGame
{
private:

	long int m_lastFrame;
	float m_frameDelta;
	bool m_isRunning;
	bool m_drawMainScene;

	/// Use std::list so iterators remain valid after registering/unregistering subgames.
    std::list<SubGame*> m_subGames;

	Profiler m_profiler;

public:

	NeoGame(void);
	virtual ~NeoGame(void);

public:

	bool isDrawingMainScene() const
	{
		return m_drawMainScene;
	}

	void setDrawMainScene(bool b)
	{
		m_drawMainScene = b;
	}

	Profiler& getProfiler() { return m_profiler; }
	inline float getFrameDelta() { return m_frameDelta; }

	// is running
	inline bool isRunning(void){ return m_isRunning; }

	// begin / end
	void begin(void) { onBegin(); onBeginLevel(); onBeginScene(); m_isRunning = true; }
	void end(void) { onEndScene(); onEndLevel(); onEnd(); m_isRunning = false; }

	// events
	virtual void update(void);

	/**
	 * @brief Draws the game.
	 *
	 * Keep in mind that this method needs to be called between startFrame and endFrame
	 * for the frame to show up correctly.
	 */
	virtual void draw(void);

	/**
	 * @brief Will be called as soon as the game starts.
	 */
	virtual void onBegin(void);

	/**
	 * @brief Will be called as soon as the game ends.
	 */
	virtual void onEnd(void);

	virtual void onBeginLevel(void) {}
	virtual void onEndLevel(void) {}

	virtual void onBeginScene(void);
	virtual void onEndScene(void);

	/**
	 * @brief Adds a SubGame to the list of registered games.
	 *
	 * This can be used to catch onBegin, update, draw and onEnd events without
	 * replacing the real NeoGame. Returns the index of the newly registered subgame.
	 * It will return the ID of the last registered game if the given object is NULL.
	 * If no game was registered it will return -1 in the case of NULL.
	 *
	 * @return The ID of the last added subgame or -1
	 */
	size_t registerSubGame(SubGame* g)
	{
		if(g)
			m_subGames.push_back(g);

		return m_subGames.size() - 1;
	}

	/**
	 * @brief Removes a SubGame from the registered games.
	 * @param g The SubGame to remove.
	 */
	void removeSubGame(SubGame* const g)
	{
		m_subGames.remove(g);
	}

	/**
	 * @brief Prepares the graphics system for the next frame.
	 */
	virtual void startFrame() {}

	/**
	 * @brief Finalizes the current frame and shows the result on the screen.
	 *
	 * Swaps buffers on systems with double buffering.
	 */
	virtual void endFrame() {}
};
}
#endif

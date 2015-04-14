//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 20014-2015 Yannick Pflanzer <www.neo-engine.de>
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

    bool m_postEffectsEnabled;
    MPostProcessor m_postProcessor;

    std::vector<SubGame*> m_subGames;

public:

	NeoGame(void);
	virtual ~NeoGame(void);

public:

	// is running
	inline bool isRunning(void){ return m_isRunning; }

    // post effects
    inline bool hasPostEffects(void){ return m_postEffectsEnabled; }
    inline void enablePostEffects() { m_postEffectsEnabled = true; }
    inline void disablePostEffects() { m_postEffectsEnabled = false; }

    inline MPostProcessor* getPostProcessor() { return &m_postProcessor; }

	// begin / end
	void begin(void) { onBegin(); onBeginLevel(); onBeginScene(); m_isRunning = true; }
	void end(void) { onEndScene(); onEndLevel(); onEnd(); m_isRunning = false; }

	// events
	virtual void update(void);
	virtual void draw(void);

	/**
	 * @brief Will be called as soon as the game starts.
	 */
	virtual void onBegin(void);

	/**
	 * @brief Will be called as soon as the game ends.
	 */
	virtual void onEnd(void);

	virtual void onBeginLevel(void) { }
	virtual void onEndLevel(void){}

	virtual void onBeginScene(void);
	virtual void onEndScene(void);

	/**
	 * @brief Adds a SubGame to the list of registered games.
	 *
	 * This can be used to catch onBegin, update, draw and onEnd events without
	 * replacing the real NeoGame.
	 */
	void registerSubGame(SubGame* g)
	{
		if(g)
			m_subGames.push_back(g);
	}
};
}
#endif

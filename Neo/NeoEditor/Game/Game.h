//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef _EDITOR_GAME_H
#define _EDITOR_GAME_H


class M_ENGINE_EXPORT EditorGame
{
private:

	bool m_isRunning;

    bool m_postEffectsEnabled;
    PostProcessor m_postProcessor;

public:

	EditorGame(void);
	virtual ~EditorGame(void);

public:

	// is running
	inline bool isRunning(void){ return m_isRunning; }

    // post effects
    inline bool hasPostEffects(void){ return m_postEffectsEnabled; }
    inline void enablePostEffects() { m_postEffectsEnabled = true; }
    inline void disablePostEffects() { m_postEffectsEnabled = false; }

    inline PostProcessor* getPostProcessor() { return &m_postProcessor; }

	// begin / end
	void begin(void) { onBegin(); onBeginLevel(); onBeginScene(); m_isRunning = true; }
	void end(void) { onEndScene(); onEndLevel(); onEnd(); m_isRunning = false; }

	// events
	virtual void update(void);
	virtual void draw(void);

	virtual void onBegin(void) { }
	virtual void onEnd(void){}

	virtual void onBeginLevel(void) { }
	virtual void onEndLevel(void){}

	virtual void onBeginScene(void);
	virtual void onEndScene(void);
};

#endif

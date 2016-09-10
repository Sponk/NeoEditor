#ifndef NEO_SDLGAME_H
#define NEO_SDLGAME_H

#include <NeoEngine.h>
#include <SDL.h>

class SDLGame : public Neo::NeoGame
{
	SDL_Window* m_window;
	SDL_GLContext m_context;
	SDL_Renderer* m_renderer;

	std::string* m_glVersion;
	std::string* m_multisample;
	std::string* m_visibleObjectsCount;

public:
	SDLGame();
	~SDLGame();

	void onBegin();
	void onEnd();

	void update();
	void draw();

	SDL_Window* getSDLWindow() const { return m_window; }

	virtual void startFrame() override { *m_visibleObjectsCount = "0"; }
	virtual void endFrame() override { SDL_GL_SwapWindow(m_window); }
};

#endif

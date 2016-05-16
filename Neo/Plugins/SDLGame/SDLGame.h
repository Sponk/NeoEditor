#ifndef NEO_SDLGAME_H
#define NEO_SDLGAME_H

#include <NeoEngine.h>
#include <SDL.h>

class SDLGame : public Neo::NeoGame
{
	SDL_Window* m_window;
	SDL_GLContext m_context;
	SDL_Renderer* m_renderer;

public:
	void onBegin();
	void onEnd();

	void update();
	void draw();

	SDL_Window* getSDLWindow() const { return m_window; }
};

#endif

#ifndef NEO_SDLINPUTCONTEXT_H
#define NEO_SDLINPUTCONTEXT_H

#include <NeoCore.h>
#include <SDL.h>

class SDLInputContext : public Neo::InputContext
{
	Neo::INPUT_KEYS translateEvent(SDL_Keycode key);

public:
	virtual void handleInput();
};

#endif //NEO_SDLINPUTCONTEXT_H

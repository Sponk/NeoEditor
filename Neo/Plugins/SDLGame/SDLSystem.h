#ifndef NEO_SDLSYSTEM_H
#define NEO_SDLSYSTEM_H

#include <NeoCore.h>

class SDLSystem : public Neo::SystemContextDummy
{
public:
	SDLSystem();
	virtual unsigned long getSystemTick(void);
	virtual void setWindowTitle(const char* title);
	virtual void sleep(long int millis);
};

#endif //NEO_SDLSYSTEM_H

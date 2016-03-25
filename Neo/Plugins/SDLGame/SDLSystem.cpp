#include "SDLSystem.h"
#include <SDL.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#if _WIN32
#include <direct.h>
#define getcwd _getcwd
#endif // _WIN32

unsigned long SDLSystem::getSystemTick(void)
{
	return SDL_GetTicks();
}

void SDLSystem::setWindowTitle(const char* title)
{

}

void SDLSystem::sleep(long int millis)
{
	if (millis <= 0)
		return;

	SDL_Delay(millis);
}

const char* SDLSystem::getWorkingDirectory()
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);
	return currentDirectory;
}

/*void NeoWindow::setCurrentDirectory(const char * directory)
{
	chdir(directory);
}*/
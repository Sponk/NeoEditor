#include "SDLSystem.h"
#include "SDLGame.h"
#include <SDL.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#if _WIN32
#include <direct.h>
#define getcwd _getcwd
#endif // _WIN32

SDLSystem::SDLSystem()
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);

	setWorkingDirectory(currentDirectory);
}

unsigned long SDLSystem::getSystemTick(void)
{
	return SDL_GetTicks();
}

void SDLSystem::setWindowTitle(const char* title)
{
	SDLGame* game = dynamic_cast<SDLGame*>(Neo::NeoEngine::getInstance()->getGame());
	if(!game)
	{
		MLOG_ERROR("Could not change window title: Game instance is NULL!");
		return;
	}

	SDL_SetWindowTitle(game->getSDLWindow(), title);
}

void SDLSystem::sleep(long int millis)
{
	if (millis <= 0)
		return;

	SDL_Delay(millis);
}

/*const char* SDLSystem::getWorkingDirectory()
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);
	return currentDirectory;
	}*/

/*void NeoWindow::setCurrentDirectory(const char * directory)
{
	chdir(directory);
}*/

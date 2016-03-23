#include <NeoEngine.h>
#include <Plugin.h>

using namespace Neo;

#define FRAME_CAP 16 // 16ms for each frame.

int main(int argc, char* argv[])
{
	Plugin scripting, gl4, behaviors, loaders, renderer, sound, sdl2;

	try
	{
		sdl2.load("SDLGame");

		scripting.load("LuaScripting");
		behaviors.load("CommonBehaviors");
		loaders.load("AssetLoading");
		sound.load("OpenALContext");

		// Needs to be loaded _after_ SDL so the GL Context is set up already.
		gl4.load("GL4Context");
		renderer.load("StandardRenderer");

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::NeoGame* game = engine->getGame();
		Neo::SystemContext* system;

		game->begin();
		//engine->loadLevel("assets/levels/test.level");
		while (engine->isActive())
		{
			system = engine->getSystemContext();
			unsigned long int time = system->getSystemTick();

			game->update();
			game->draw();

			system->sleep(FRAME_CAP - (system->getSystemTick() - time));
		}
		game->end();
	}
	catch(std::exception& e)
	{
		MLOG_ERROR("Exception: " << e.what());
	}

	return 0;
}

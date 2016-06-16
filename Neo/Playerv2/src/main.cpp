#include <NeoEngine.h>
#include <Player.h>
#include <sexpresso.hpp>
#include <memory>

using namespace Neo;

#define FRAME_CAP 16 // 16ms for each frame.
void registerDebugHandler();

int main(int argc, char* argv[])
{
	Player player(FRAME_CAP);
	registerDebugHandler();

	std::string err;
	char* config = readTextFile("assets/plugins.cfg");

	if(!config)
	{
		MLOG_WARNING("Could not open plugin config!");
		return 1;
	}

	std::string confstr = config;
	free(config);

	sexpresso::Sexp tree;

	tree = sexpresso::parse(confstr, err);
	sexpresso::Sexp* pluginlist = tree.getChildByPath("project/plugins");
    //sexpresso::Sexp* startlevel = tree.getChildByPath("project/level");

	try
	{
		for(auto&& s : pluginlist->arguments())
		{
			if(s.isNil() || s.isSexp())
				continue;

			player.loadPlugin(s.toString().c_str());
		}

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::NeoGame* game = engine->getGame();
		Neo::SystemContext* system;

		if(!game)
			engine->setGame(game = new NeoGame);

        system = engine->getSystemContext();
        system->setArgc(argc);
        system->setArgv(&argv);

		player.begin();
		player.execute();
		player.end();
/*		game->begin();

        //char scriptfile[512];
        //getGlobalFilename(scriptfile, engine->getSystemContext()->getWorkingDirectory(), startlevel->value.sexp[1].toString().c_str());
        //engine->loadLevel(scriptfile);
        engine->loadLevel("");

		while (engine->isActive())
		{
			system = engine->getSystemContext();
			unsigned long int time = system->getSystemTick();

			game->update();
			game->draw();

			system->sleep(FRAME_CAP - (system->getSystemTick() - time));
		}
		game->end();*/
	}
	catch(std::exception& e)
	{
		MLOG_ERROR("Exception: " << e.what());
	}

	return 0;
}

#include <NeoEngine.h>
#include <sexpresso.hpp>
#include <memory>

using namespace Neo;

#define FRAME_CAP 16 // 16ms for each frame.
void registerDebugHandler();

int main(int argc, char* argv[])
{
	registerDebugHandler();

	std::string err;
	char* config = readTextFile("assets/plugins.cfg");

	std::string confstr = config;
	free(config);

	sexpresso::Sexp tree;

	if(!config)
	{
		MLOG_ERROR("Could not open config file!");
		return 1;
	}

	tree = sexpresso::parse(confstr, err);
	sexpresso::Sexp* pluginlist = tree.getChildByPath("project/plugins");
	sexpresso::Sexp* startlevel = tree.getChildByPath("project/level");

	try
	{
		std::vector<std::shared_ptr<Plugin>> plugins;
		for(auto&& s : pluginlist->arguments())
		{
			if(s.isNil() || s.isSexp())
				continue;

			std::shared_ptr<Plugin> p = std::make_shared<Plugin>();
			p->load(s.toString().c_str());
			plugins.push_back(p);
		}

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::NeoGame* game = engine->getGame();
		Neo::SystemContext* system;

		if(!game)
			engine->setGame(game = new NeoGame);

		game->begin();

		char scriptfile[512];
		getGlobalFilename(scriptfile, engine->getSystemContext()->getWorkingDirectory(), startlevel->value.sexp[1].toString().c_str());
		engine->loadLevel(scriptfile);

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

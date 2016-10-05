#include <NeoEngine.h>
#include <Player.h>
#include <sexpresso.hpp>
#include <memory>
#include <Project.h>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace Neo;

#define FRAME_CAP 16 // 16ms for each frame.
void registerDebugHandler();

int main(int argc, char* argv[])
{
	Player player(FRAME_CAP);
	registerDebugHandler();

	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Project project;

	Plugin npk;
	npk.load("NPKPlugin");
	
	if(isFileExist("assets.npk"))
	{
		engine->getPackageManager()->loadPackage("assets.npk");
		MLOG_INFO("Found assets.npk!");

		if (!project.load("project.nproj"))
		{
			MLOG_ERROR("Could not load project!");
			return 1;
		}
	}
	else
	{
		MLOG_INFO("Didn't find assets.npk, loading the first project found!");
		vector<string> files;
		readDirectory("./", files, false, false);

		bool loaded = false;
		for(auto s : files)
			if(s.find(".nproj") != -1)
			{
				project.load(s.c_str());
				loaded = true;
				break;
			}

		if(!loaded)
		{
			MLOG_ERROR("No project was found!");
			return 1;
		}
	}

	MLOG_INFO("Loaded project: " << project.getName());

	try
	{
		for(auto s : project.getPlugins())
		{
			player.loadPlugin(s.c_str());
		}

		{
			Neo::SystemContext* system = engine->getSystemContext();
			char repertory[256];
			getcwd(repertory, sizeof(repertory));
			MLOG_INFO("Working directory: " << repertory);
			system->setWorkingDirectory(repertory);

			system->setArgc(argc);
			system->setArgv(&argv);
		}

		Neo::NeoGame* game = engine->getGame();
		if(!game)
			engine->setGame(game = new NeoGame);

		player.begin(project.getLevel().c_str());
		player.execute();
		player.end();
	}
	catch(std::exception& e)
	{
		MLOG_ERROR("Exception: " << e.what());
	}

	return 0;
}

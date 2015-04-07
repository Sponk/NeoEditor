#include <iostream>
#include <string>

#include <NeoEngine.h>
using namespace Neo;

#include <LevelLoad.h>
#include <LevelSave.h>
#include <MeshLoad.h>
#include <MeshSave.h>
#include <ImageLoader.h>
#include <SoundLoader.h>
#include <FreetypeLoader.h>
#include <BinFontLoader.h>
#include <BinMeshLoader.h>

#include <DummyContext.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include <BulletContext.h>
#include <LuaScript.h>

#include <PackageManagerNPK.h>
#include <Project.h>
#include "Publish/Publisher.h"
#include <AssimpMeshLoader.h>

#include <Window/Window.h>
#include <unistd.h>

using namespace std;

#define VERSION "0.1"

#ifndef WIN32
#define SEPERATOR "/"
#else
#define SEPERATOR "\\"
#endif

struct Settings
{
	string outputDirectory;
	string inputProject;
	string startLevel;
	string player;
	bool printHelp;
};

Settings parseOpt(int argc, char* argv[])
{
	Settings s;
	if(argc <= 1)
	{
		s.printHelp = true;
		return s;
	}

	s.printHelp = false;

#ifndef WIN32
	s.outputDirectory = "./published";
	s.player = "NeoPlayer";
#else
	s.outputDirectory = ".\\published";
	s.player = "NeoPlayer.exe";
#endif
	
	string arg;
	string opt;
	for (int i = 1; i < argc; i++)
	{
		arg = argv[i];

		if (arg == "-i")
		{
			i++;
			s.inputProject = argv[i];
		}

		else if (arg == "-o")
		{
			i++;
			s.outputDirectory = argv[i];
		}

		else if (arg == "-s")
		{
			i++;
			s.startLevel = argv[i];
		}

		else if (arg == "-p")
		{
			i++;
			s.player = argv[i];
		}

		else if (arg == "-h")
		{
			s.printHelp = true;
			return s;
		}

		else
		{
			cout << "Unknown option '" << arg << "'" << endl;
			s.printHelp = true;
			return s;
		}
	}

	return s;
}

void printHelp()
{
	cout << "Usage: neo-publish -i <project> -o <output> -s <startlevel> -p <player>"
		 << endl;
	cout << "\t\t-i\tInput project file" << endl;
	cout << "\t\t-o\tOutput directory. Defaults to \"./published\"" << endl;
	cout << "\t\t-s\tStart level. Defaults to the one specified in the project "
			"file." << endl;
	cout << "\t\t-p\tThe executable player file. This is crossplatform." << endl;
	cout << "\t\t-h\tPrint this help." << endl;
	cout << "This tool has puffin powers!" << endl;
}

bool loadProject(Settings s)
{
	Project project;
	NeoEngine* engine = NeoEngine::getInstance();
	const char* filename = s.inputProject.c_str();

	// load project file
	if (!project.loadXML(filename))
	{
		cout << "Can't load specified project!" << endl;
		return false;
	}

	if (s.startLevel.empty())
	{
		s.startLevel = project.startLevel;
	}

	Level* level = engine->getLevel();
	if (!engine->getLevelLoader()->loadData(s.startLevel.c_str(), level))
	{
		cout << "Could not load start level!" << endl;
		return false;
	}

	return true;
}

void engineSetup()
{
	NeoEngine* engine = NeoEngine::getInstance();
	NeoGame* game = new NeoGame();
	LuaScript* script = new Neo::LuaScript();
	Level* level = new Level();
	SystemContext* context = (SystemContext*)new Neo::MWinContext();
	MPackageManager* pmanager = new Neo::MPackageManagerNPK;
	BulletContext* physcontext = new Neo::BulletContext();
	DummyContext* render = new DummyContext();

	pmanager->init();

	level->addNewScene();
	level->setCurrentSceneId(0);

	engine->setLevel(level);
	engine->setScriptContext(script);
	engine->setSystemContext(context);
	engine->setGame(game);
	engine->setPackageManager(pmanager);
	engine->setPhysicsContext(physcontext);
	engine->setRenderingContext(render);

	// loaders
	engine->getImageLoader()->addLoader(M_loadImage);  // image loader
	engine->getSoundLoader()->addLoader(M_loadSound);  // sound loader
	engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader
	engine->getFontLoader()->addLoader(M_loadFont);	// font loader
	engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader

	// mesh loaders
	engine->getMeshLoader()->addLoader(xmlMeshLoad);
	engine->getMeshLoader()->addLoader(M_loadBinMesh);
	engine->getMeshLoader()->addLoader(M_loadAssimpMesh);
	engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
	engine->getArmatureAnimLoader()->addLoader(M_loadBinArmatureAnim);
	engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
	engine->getTexturesAnimLoader()->addLoader(M_loadBinTexturesAnim);
	engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
	engine->getMaterialsAnimLoader()->addLoader(M_loadBinMaterialsAnim);

	engine->setLevel(new Level);
}

int main(int argc, char* argv[])
{
	cout << "NeoPublish v" << VERSION << endl;
	
	Settings s = parseOpt(argc, argv);

	if (s.printHelp)
	{
		cout << "No project file specified!" << endl;
		printHelp();
		return 0;
	}

	engineSetup();
	if(!loadProject(s))
		return 1;

	cout << "Publishing from: " << s.inputProject << endl;
	cout << "Publishing to: " << s.outputDirectory << endl;
	cout << "Using player: " << s.player << endl;

	string workingDirectory = s.outputDirectory;
	if(int idx = workingDirectory.find_last_of(SEPERATOR))
		workingDirectory.erase(idx);

	
	string dataDirectory = argv[0];
	if(int idx = dataDirectory.find_last_of(SEPERATOR))
		dataDirectory.erase(idx);

	chdir(dataDirectory.c_str());
	NeoWindow::getInstance()->setWorkingDirectory(workingDirectory.c_str());
	NeoWindow::getInstance()->setCurrentDirectory(".");
    
	Publisher* publisher = Publisher::getInstance();
	publisher->publish(s.inputProject.c_str(), s.outputDirectory.c_str(),
					   s.player.c_str());

	cout << "Project was successfully published!" << endl;
	return 0;
}

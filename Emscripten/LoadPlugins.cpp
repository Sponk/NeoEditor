#include <NeoEngine.h>
#include <Player.h>
#include <stdio.h>
#include <emscripten.h>
#include <setjmp.h>
#include <Project.h>

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_SDL
#define EndPlugin EndPlugin_SDL

#include "../Neo/Plugins/SDLGame/Plugin.cpp"

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_GLES
#define EndPlugin EndPlugin_GLES

#include "../Neo/Plugins/GLESRenderer/Plugin.cpp"

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_AssetLoading
#define EndPlugin EndPlugin_AssetLoading

#include "../Neo/Plugins/AssetLoading/Plugin.cpp"

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_LuaScripting
#define EndPlugin EndPlugin_LuaScripting

#include "../Neo/Plugins/LuaScripting/Plugin.cpp"

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_NPK
#define EndPlugin EndPlugin_NPK

#include "../Neo/Plugins/NPK/Plugin.cpp"

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_Bullet
#define EndPlugin EndPlugin_Bullet

#include "../Neo/Plugins/BulletPhysics/Plugin.cpp"

#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_OpenAL
#define EndPlugin EndPlugin_OpenAL

#include "../Neo/Plugins/OpenALContext/Plugin.cpp"

/*#undef StartPlugin
#undef EndPlugin
#define StartPlugin StartPlugin_Editor
#define EndPlugin EndPlugin_Editor

#include "../Neo/Plugins/NeoEditor/src/Plugin.cpp"*/

#undef StartPlugin
#undef EndPlugin 

using namespace Neo;
Player player(0);

void loop()
{
	auto engine = NeoEngine::getInstance();

	if(!engine->getGame()->isRunning())
		return;
	
	auto renderer = engine->getRenderer();
	//renderer->clearScreen(Vector3(1,0,0));
	

	player.executeFrame();
	
	/*NeoGame* game = NeoEngine::getInstance()->getGame();
	game->update();
	game->startFrame();
	game->draw();
	
	renderer->set2D(Vector2(1024, 768));
	renderer->drawColoredQuad(Vector2(50,50), Vector2(200,200), Vector4(0,1,0,1), 0);
	
	game->endFrame();*/
}

/*extern "C" int _setjmp(jmp_buf env)
{
	return 0;
}

extern "C" void _longjmp(jmp_buf env, int value)
{
	
}*/

int main(int argc, char* argv[])
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Neo::NeoGame* game = engine->getGame();
	Neo::SystemContext* system;

	StartPlugin_SDL();
	StartPlugin_GLES();
	StartPlugin_AssetLoading();
	StartPlugin_LuaScripting();
	StartPlugin_NPK();
	StartPlugin_Bullet();
	StartPlugin_OpenAL();
	// StartPlugin_Editor();

/*	WebPackage package;
	engine->setPackageManager(&package);

	package.init();
	package.loadPackage("http://127.0.0.1:8000/");*/
	
	/*if(!game)
	  engine->setGame(game = new NeoGame);*/
	
	system = engine->getSystemContext();
	system->setArgc(argc);
	system->setArgv(&argv);

	system->setWorkingDirectory("/");
	engine->getConfigurationRegistry().setVariable("g_rendererType", "gles");
		
	std::string fullpath = "http://127.0.0.1:8000/assets.npk";
	emscripten_async_wget(fullpath.c_str(), "assets.npk",
						  [](const char* s){
							  MLOG_INFO("Done downloading " << s);

							  NeoEngine* engine = NeoEngine::getInstance();
							  engine->getPackageManager()->loadPackage("assets.npk");

							  Project project;
							  if (!project.load("project.nproj"))
							  {
								  MLOG_ERROR("Could not load project!");
								  return exit(-1);
							  }

							  MLOG_INFO("Loaded project: " << project.getName());

							  engine->getGame()->begin();
							  engine->loadLevel(project.getLevel().c_str());
							  // engine->getScriptContext()->runString("print(tostring(Neo2DLua.Widget))");
						  },

						  [](const char* s){
							  MLOG_INFO("Error: Could not download " << s);
							  exit(-1);
						  });
	
	emscripten_set_main_loop(loop, 0, 1);
	
//	player.execute();
//	player.end();
	
//	EndPlugin_SDL();
//	EndPlugin_GLES();
}

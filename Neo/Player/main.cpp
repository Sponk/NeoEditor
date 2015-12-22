//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================
//jan 2012 - Philipp Geyer <philipp@geyer.co.uk> - embedded project/package manager

#include <GameWinEvents.h>
#include <NeoEngine.h>
#include <SDLThread.h>
#include <algorithm>

#include "Backend/PlayerBackend.h"

// NeoGui
#include <Neo2DEngine.h>
#include <Window/Window.h>

using namespace Neo;

#ifdef main
#undef main
#endif

// Don't show cmd window
#ifdef _MSC_VER
//#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

// Define default start values
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768
#define DEFAULT_FULLSCREEN false

// window events
void windowEvents ( MWinEvent * windowEvents )
{
	NeoEngine * engine = NeoEngine::getInstance();

	// game
	NeoGame * game = engine->getGame();
	if ( game )
	{
		if ( game->isRunning() )
		{
			gameWinEvents ( windowEvents );
		}
	}

	if ( windowEvents->type == MWIN_EVENT_WINDOW_CLOSE )
	{
		engine->setActive ( false );
	}
}

// update
void update ( void )
{
	PlayerBackend::getInstance()->logicLoop();
}

// draw
void draw ( void )
{
	PlayerBackend::getInstance()->graphicLoop();
	NeoWindow::getInstance()->swapBuffer();
}

// FIXME: In header file
void registerDebugHandler();

#ifdef ANDROID
extern "C" void player_main()
{

}
#endif

struct Parameters
{
	Parameters() {}
	Parameters ( std::string script, std::string proj, unsigned int width,
	             unsigned int height, bool f ) :
		script ( script ),
		project ( proj ),
		width ( width ),
		height ( height ),
		fullscreen ( f ) {}

	std::string script;
	std::string project;
	unsigned int width;
	unsigned int height;
	bool fullscreen;
};

Parameters parseParameters ( int argc, char** argv )
{
	Parameters p ( "assets/main.lua", "", DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_FULLSCREEN );

	for ( int i = 1; i < argc; i++ )
	{
		if ( !strcmp ( argv[i], "-e" ) )
			p.script = argv[++i];
		else if ( !strcmp ( argv[i], "-p" ) )
			p.project = argv[++i];
		else if ( !strcmp ( argv[i], "-r" ) )
		{
			sscanf ( argv[++i], "%d", &p.width );
			sscanf ( argv[++i], "%d", &p.height );
		}
		else if ( !strcmp ( argv[i], "-f" ) )
			p.fullscreen = true;

	}

	return p;
}

// main
int main ( int argc, char **argv )
{
#ifndef ANDROID
	setlocale ( LC_NUMERIC, "C" );
	registerDebugHandler();
#endif

	Parameters params = parseParameters ( argc, argv );
	MLOG_INFO ( "Neo Player version:\t" << PLAYER_VERSION_STRING );

	// get engine (first time call constructor)
	NeoEngine * engine = NeoEngine::getInstance();

	// get window (first time call constructor)
	NeoWindow * window = NeoWindow::getInstance();

	// get the backend (first time call constructor)
	PlayerBackend * backend = PlayerBackend::getInstance();

	// Init default thread
	ThreadFactory* mgr = ThreadFactory::getInstance();
	mgr->setTemplateSemaphore ( new SDLSemaphore() );
	mgr->setTemplateThread ( new SDLThread() );

	// create window
	if ( !window->create ( std::string ( "Neo " ).append ( PLAYER_VERSION_STRING ).c_str(), params.width, params.height, 32, params.fullscreen ) )
	{
		MLOG_ERROR ( "cannot create window" );
		return 0;
	}

	//backend->start();

	if ( params.fullscreen )
		window->hideCursor();

	// set current directory
	char rep[256];
	getRepertory ( rep, argv[0] );
	window->setCurrentDirectory ( rep );

	// window pointer event
	window->setPointerEvent ( windowEvents );

	// load project
	bool projectFound = false;
	if ( argc > 1 )
	{
		char filename[256];
		getGlobalFilename ( filename, window->getCurrentDirectory(), argv[1] );
		if ( backend->loadProject ( filename ) )
		{
			// Initialize GUI bindings
			Neo2D::Neo2DEngine::getInstance()->setupLuaInterface ( NeoEngine::getInstance()->getScriptContext() );

			//engine->getGame()->begin();
			projectFound = true;
			MLOG_INFO ( "Found project: " << filename );
		}
	}

	if ( ! projectFound )
	{
		// if we've found, and written the embedded sections, then use that data
		if ( ( strstr ( s_embedded_level_name, "[EMBEDDED" ) != s_embedded_level_name ) &&
		        ( strstr ( s_embedded_renderer, "[EMBEDDED" ) != s_embedded_renderer ) )
		{
			Project embeddedProj;
			embeddedProj.renderer = s_embedded_renderer;

			char levelName[256];
			getGlobalFilename ( levelName, window->getCurrentDirectory(), s_embedded_level_name );
			char projName[256];
			getGlobalFilename ( projName, window->getCurrentDirectory(), s_embedded_game_name );

			embeddedProj.startLevel = levelName;

			Neo2D::Neo2DEngine::getInstance()->setupLuaInterface ( engine->getScriptContext() );

			backend->loadProject ( &embeddedProj, projName );

			// This needs to be done in the update thread
			// engine->getGame()->begin();
			projectFound = true;
			MLOG_INFO ( "Found embedded project: " << projName );
		}
		else
		{
			// look for an mproj in the current directory
			std::vector<std::string> files;
			readDirectory ( ".", files );

			for ( int i = 0; i < files.size(); ++i )
			{
				if ( strstr ( files[i].c_str(), ".mproj" ) )
				{
					char filename[256];
					getGlobalFilename ( filename, window->getCurrentDirectory(), files[i].c_str() );

					if ( backend->loadProject ( filename ) )
					{
						// Initialize GUI bindings
						Neo2D::Neo2DEngine::getInstance()->setupLuaInterface ( engine->getScriptContext() );

						// This needs to be done in the update thread
						//engine->getGame()->begin();
						projectFound = true;
						MLOG_INFO ( "Found project: " << filename );
						break;
					}
				}
			}
		}
	}

	// create the update thread
	SDLThread updateThread;
	Neo2D::Neo2DEngine* guiSystem = Neo2D::Neo2DEngine::getInstance();

	if ( !projectFound )
	{
		MLOG_INFO ( "No project found. Launching empty level." );
		backend->start();
		backend->setupEmptyProject();
	}

	//window->getUpdateSemaphore()->WaitAndLock();
	NeoGame* game = engine->getGame();
	game->begin();

	engine->getScriptContext()->runScript ( params.script.c_str() );

//    Messenger* messenger = Messenger::getInstance();
//    messenger->addInbox("MainThread", 0);

	SystemContext* system = engine->getSystemContext();
	unsigned int frameStart = 0;

	// on events
	while (engine->isActive())
	{
		frameStart = system->getSystemTick();
		window->onEvents();

		if (!engine->isActive())
		{
			game->end();
			break;
		}

		if (window->getFocus())
		{
			int tick = system->getSystemTick();
			update();
			draw();
		}
		else
		{
			update();
			draw();

			guiSystem->draw();
			window->sleep ( 100.0 );
			continue;
		}

		// update postponed requests
		NeoEngine::getInstance()->updateRequests();

		// TODO: Make target frame time configurable!
		window->sleep ( 16.6 - ( system->getSystemTick() - frameStart ) );
	}

	backend->clear();
	return 0;
}

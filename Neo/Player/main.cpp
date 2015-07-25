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

// window events
void windowEvents(MWinEvent * windowEvents)
{
	NeoEngine * engine = NeoEngine::getInstance();

	// game
	NeoGame * game = engine->getGame();
	if(game)
	{
		if(game->isRunning())
        {
			gameWinEvents(windowEvents);
		}
    }

	if(windowEvents->type == MWIN_EVENT_WINDOW_CLOSE)
    {
		engine->setActive(false);
    }
}

// update
void update(void)
{
	// Update Neo2D before the rest so all input is still in the pipeline
	Neo2D::Neo2DEngine::getInstance()->update();
	PlayerBackend::getInstance()->logicLoop();
}

// draw
void draw(void)
{
	PlayerBackend::getInstance()->graphicLoop();
	Neo2D::Neo2DEngine::getInstance()->draw();
    NeoWindow::getInstance()->swapBuffer();
}

// FIXME: In header file
void registerDebugHandler();

// main
int main(int argc, char **argv)
{
#ifndef ANDROID
	setlocale(LC_NUMERIC, "C");
#endif

	registerDebugHandler();
	
	MLOG_INFO("Neo Player version:\t" << PLAYER_VERSION_STRING);

	unsigned int width = 1024;
	unsigned int height = 768;
	int fullscreen = false;

	if(argc > 2)
		sscanf(argv[2], "%d", &width);
	if(argc > 3)
		sscanf(argv[3], "%d", &height);
	if(argc > 4)
		sscanf(argv[4], "%d", &fullscreen);

	// get engine (first time call constructor)
	NeoEngine * engine = NeoEngine::getInstance();

	// get window (first time call constructor)
	NeoWindow * window = NeoWindow::getInstance();

	// get the backend (first time call constructor)
	PlayerBackend * backend = PlayerBackend::getInstance();

	// Init default thread
	ThreadFactory* mgr = ThreadFactory::getInstance();
	mgr->setTemplateSemaphore(new SDLSemaphore());
	mgr->setTemplateThread(new SDLThread());

	// create window
	if(!window->create(std::string("Neo ").append(PLAYER_VERSION_STRING).c_str(), width, height, 32, fullscreen == 1))
	{
		MLOG_ERROR("cannot create window");
		return 0;
	}

	//backend->start();

	if(fullscreen)
		window->hideCursor();

	// set current directory
	char rep[256];
	getRepertory(rep, argv[0]);
	window->setCurrentDirectory(rep);

	// window pointer event
	window->setPointerEvent(windowEvents);

	// load project
	bool projectFound = false;
	if(argc > 1)
    {
		char filename[256];
		getGlobalFilename(filename, window->getCurrentDirectory(), argv[1]);
		if(backend->loadProject(filename))
		{
            // Initialize GUI bindings
			Neo2D::Neo2DEngine::getInstance()->setupLuaInterface(NeoEngine::getInstance()->getScriptContext());

	    	//engine->getGame()->begin();
			projectFound = true;
		}
	}

	if(! projectFound)
	{
		// if we've found, and written the embedded sections, then use that data
		if((strstr(s_embedded_level_name, "[EMBEDDED") != s_embedded_level_name) &&
		   (strstr(s_embedded_renderer, "[EMBEDDED") != s_embedded_renderer))
		{
			Project embeddedProj;
			embeddedProj.renderer = s_embedded_renderer;

			char levelName[256];
			getGlobalFilename(levelName, window->getCurrentDirectory(), s_embedded_level_name);
			char projName[256];
			getGlobalFilename(projName, window->getCurrentDirectory(), s_embedded_game_name);

			embeddedProj.startLevel = levelName;

			Neo2D::Neo2DEngine::getInstance()->setupLuaInterface(engine->getScriptContext());

			backend->loadProject(&embeddedProj, projName);

			// This needs to be done in the update thread
			// engine->getGame()->begin();
			projectFound = true;
		}
		else
		{
			// look for an mproj in the current directory
			std::vector<std::string> files;
			readDirectory(".", &files);

			for(int i = 0; i < files.size(); ++i)
			{
				if(strstr(files[i].c_str(), ".mproj"))
				{
					char filename[256];
					getGlobalFilename(filename, window->getCurrentDirectory(), files[i].c_str());

					if(backend->loadProject(filename))
					{
                        // Initialize GUI bindings
						Neo2D::Neo2DEngine::getInstance()->setupLuaInterface(engine->getScriptContext());

						// This needs to be done in the update thread
						//engine->getGame()->begin();
						projectFound = true;
						break;
					}
				}
			}
		}
	}

	// create the update thread
    SDLThread updateThread;
    Neo2D::Neo2DEngine* guiSystem = Neo2D::Neo2DEngine::getInstance();

	if(!projectFound)
	{
		backend->start();
		backend->setupEmptyProject();
	}
	
	//window->getUpdateSemaphore()->WaitAndLock();
	NeoGame* game = engine->getGame();
	game->begin();

	engine->getScriptContext()->runScript("assets/main.lua");
	
//    Messenger* messenger = Messenger::getInstance();
//    messenger->addInbox("MainThread", 0);

	SystemContext* system = engine->getSystemContext();
	unsigned int frameStart = 0;

    bool isActive = true;
    // on events
    while(isActive)
    {
    	frameStart = system->getSystemTick();
		window->onEvents();

        if(!isActive)
        {
            game->end();
			break;
        }

        if(window->getFocus())
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
            window->sleep(100.0);
			continue;
        }

        isActive = engine->isActive();

        // update postponed requests
        NeoEngine::getInstance()->updateRequests();

        // TODO: Make target frame time configurable!
        window->sleep(16.6 - (system->getSystemTick() - frameStart));
    }

	backend->clear();
	return 0;
}

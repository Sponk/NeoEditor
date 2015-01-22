/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisPlayer
// main.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#include <NeoEngine.h>
#include <MWindow.h>
#include <SDLThread.h>

#include <MGameWinEvents.h>
#include "Maratis/MaratisPlayer.h"

// NeoGui
#include <GuiSystem.h>

using namespace Neo;

#ifdef main
    #undef main
#endif

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
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
	MaratisPlayer::getInstance()->logicLoop();
	Neo::Gui::GuiSystem::getInstance()->update();
}

// draw
void draw(void)
{
	MaratisPlayer::getInstance()->graphicLoop();
	Neo::Gui::GuiSystem::getInstance()->draw();
    MWindow::getInstance()->swapBuffer();
}

SDLSemaphore updateSemaphore;
bool updateThreadRunning = true;
int profiler = false;

// TODO: Abstraction!
double frametime = 0;
double framecount = -1;
int framemax = 0;
int framemin = -1;

double updatetime = 0;
double updatecount = -1;
int updatemax = 0;
int updatemin = -1;

int update_thread(void* nothing)
{
    MWindow * window = MWindow::getInstance();

	Messenger* messenger = Messenger::getInstance();
	messenger->addInbox("MainThread", 0);

	//window->getUpdateSemaphore()->WaitAndLock();
	//NeoEngine::getInstance()->getGame()->begin();
	//window->getUpdateSemaphore()->Unlock();

    // time
    int frequency = 60;
    int skipTicks = 1000 / frequency - 5;
    long previousFrame = 0;
    long startTick = window->getSystemTick();

    while(updateThreadRunning)
    {
        // Get input
        NeoEngine::getInstance()->getInputContext()->flush();
        window->onEvents();

		window->getUpdateSemaphore()->WaitAndLock();
        if(window->getFocus())
        {
            // compute target tick
            unsigned long currentTick = window->getSystemTick();

            unsigned long tick = currentTick - startTick;
            unsigned long frame = (long)(tick * (frequency * 0.001f));

            // update elapsed time
            unsigned int i;
            unsigned int steps = (int)(frame - previousFrame);

            // don't wait too much
            if(steps >= (frequency/2))
            {
               update();
               unsigned int oldTick = currentTick;
               currentTick = window->getSystemTick();

                previousFrame = frame;

                int sleep = skipTicks - (currentTick - oldTick);

				if (profiler)
				{
					int time = currentTick - oldTick;
					updatecount++;
					updatetime += time;

					if (time > updatemax)
					{
						updatemax = time;
					}
					else if (time < updatemin || updatemin < 0)
					{
						updatemin = time;
					}
				}

				window->getUpdateSemaphore()->Unlock();
                window->sleep(sleep);

                continue;
            }

            // update
            for(i=0; i<steps; i++)
            {
                update();
                previousFrame++;
            }

            unsigned int oldTick = currentTick;
            currentTick = window->getSystemTick();

            int sleep = skipTicks - (currentTick - oldTick);

			if (profiler)
			{
				int time = currentTick - oldTick;
				updatecount++;
				updatetime += time;

				if (time > updatemax)
				{
					updatemax = time;
				}
				else if (time < updatemin || updatemin < 0)
				{
					updatemin = time;
				}
            }

			window->getUpdateSemaphore()->Unlock();
            window->sleep(sleep);
        }
        else
        {
			window->getUpdateSemaphore()->Unlock();
			window->sleep(100);
        }

        fflush(stdout);
	}

	NeoEngine::getInstance()->getGame()->end();

    return 0;
}

// TODO: Make profiler a compiler option
// main
int main(int argc, char **argv)
{
#ifndef ANDROID
	setlocale(LC_NUMERIC, "C");
#endif
	
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
    if(argc > 5)
        sscanf(argv[5], "%d", &profiler);
	
	// get engine (first time call onstructor)
	NeoEngine * engine = NeoEngine::getInstance();
	
	// get window (first time call onstructor)
	MWindow * window = MWindow::getInstance();

	// create window
	if(! window->create(std::string("Neo ").append(PLAYER_VERSION_STRING).c_str(), width, height, 32, fullscreen == 1))
	{
		MLOG_ERROR("cannot create window");
		return 0;
	}

	if(fullscreen)
		window->hideCursor();
	
	// set current directory
	char rep[256];
	getRepertory(rep, argv[0]);
	window->setCurrentDirectory(rep);
	
	// get Maratis (first time call onstructor)
	MaratisPlayer * maratis = MaratisPlayer::getInstance();

	// window pointer event
	window->setPointerEvent(windowEvents);

	// load project
	bool projectFound = false;
	if(argc > 1)
    {
		char filename[256];
		getGlobalFilename(filename, window->getCurrentDirectory(), argv[1]);
		if(maratis->loadProject(filename))
		{
            // Initialize GUI bindings
			Neo::Gui::GuiSystem::getInstance()->setupLuaInterface(NeoEngine::getInstance()->getScriptContext());
			engine->getGame()->begin();
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
			
			Neo::Gui::GuiSystem::getInstance()->setupLuaInterface(NeoEngine::getInstance()->getScriptContext());
			maratis->loadProject(&embeddedProj, projName);
			
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

					if(maratis->loadProject(filename))
					{
                        // Initialize GUI bindings
						Neo::Gui::GuiSystem::getInstance()->setupLuaInterface(NeoEngine::getInstance()->getScriptContext());

						// This needs to be done in the update thread
						// engine->getGame()->begin();
						projectFound = true;
						break;
					}
				}
			}
		}
	}
	
    if(profiler)
        MLOG_INFO("Profiling enabled");

	// Init default thread
	MThreadManager* mgr = MThreadManager::getInstance();
	mgr->setTemplateSemaphore(new SDLSemaphore());
	mgr->setTemplateThread(new SDLThread());

	window->createSemaphores();

    // create the update thread
    SDLThread updateThread;
	Neo::Gui::GuiSystem* guiSystem = Neo::Gui::GuiSystem::getInstance();

	//window->getUpdateSemaphore()->WaitAndLock();
	NeoEngine::getInstance()->getGame()->begin();
    //window->getUpdateSemaphore()->Unlock();

	// Init thread
    updateThread.Start(update_thread, "Update", NULL);

    Messenger* messenger = Messenger::getInstance();
    messenger->addInbox("MainThread", 0);

    bool isActive = true;
    // on events
    while(isActive)
    {
		window->getUpdateSemaphore()->WaitAndLock();
        window->onWindowEvents();

		// Handle all messages
		while(messenger->getMessagesCount("MainThread") != 0)
		{
			Message msg = messenger->getNextMessage("MainThread");

			// loadTexture has ID 1
			if(msg.message == "loadTexture")
			{
				TextureRef* tex = engine->getLevel()->loadTexture((const char*) msg.data);
				messenger->sendMessage("Done.", 1, (void*) tex, msg.sender.c_str(), "MainThread");
			}
		}

		//MLOG_INFO("DRAW");

        if(!isActive)
        {
            engine->getGame()->end();
			window->getUpdateSemaphore()->Unlock();
			break;
        }

        if(window->getFocus())
        {
            int tick = engine->getSystemContext()->getSystemTick();
            draw();

			if (profiler)
			{
				int time = engine->getSystemContext()->getSystemTick() - tick;
				framecount++;
				frametime += time;

				if (time > framemax)
				{
					framemax = time;
				}
				else if (time < framemin || framemin < 0)
				{
					framemin = time;
				}
			}
        }
        else
        {
            draw();
			guiSystem->draw();
			window->getUpdateSemaphore()->Unlock();
            window->sleep(100);
			continue;
        }

        isActive = engine->isActive();

        // update postponed requests
        NeoEngine::getInstance()->updateRequests();
		window->getUpdateSemaphore()->Unlock();

        window->sleep(5);
        //window->sleep(0.001); // 1 mili sec seems to slow down on some machines...
    }

    updateThreadRunning = false;
    int ret = updateThread.WaitForReturn();

	MLOG_INFO("Update thread returned with exit code " << ret);

	if (profiler)
	{
		using namespace std;

		float avUpdate = (float)updatetime / (float)updatecount;
		float avFrame = (float)frametime / (float)framecount;

		cout << "\nProfiling report\n";
		cout << "******************************************************\n";

		cout << "Graphics data:\n";
		cout << "----------------\n";

		cout << "Number of frames: " << framecount << "\n";
		cout << "Average frametime: " << avFrame << " ms\n";
		cout << "Average framerate: " << 1000 / avFrame << " FPS\n";
		cout << "Max: " << framemax << " ms\n";
		cout << "Min: " << framemin << " ms\n";

		cout << "\nUpdate data (scripts, physics, etc.):\n";
		cout << "-----------------------------------------\n";
		cout << "Number of frames: " << updatecount << "\n";
		cout << "Average frametime: " << avUpdate << " ms\n";
		cout << "Average framerate: " << 1000 / avUpdate << " FPS\n";
		cout << "Max: " << updatemax << " ms\n";
		cout << "Min: " << updatemin << " ms\n";

		if (avUpdate > avFrame)
		{
			cout << "\nResult: Your application is CPU capped!\n";
		}
		else
		{
			cout << "\nResult: Your application is GPU capped!\n";
		}
	}

	maratis->clear();
	return 0;
}

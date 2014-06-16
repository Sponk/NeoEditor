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


#include <MEngine.h>
#include <MLog.h>
#include <MWindow.h>
#include <MThread.h>

#include <MGameWinEvents.h>
#include "Maratis/MaratisPlayer.h"
#include "MSchedule/MSchedule.h"

#ifdef main
    #undef main
#endif

// window events
void windowEvents(MWinEvent * windowEvents)
{
	MEngine * engine = MEngine::getInstance();

	// game
	MGame * game = engine->getGame();
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
}

// draw
void draw(void)
{
	MaratisPlayer::getInstance()->graphicLoop();
	MWindow::getInstance()->swapBuffer();
}

MSemaphore updateSemaphore;
MSemaphore inputSemaphore;
bool updateThreadRunning = true;

int update_thread(void* nothing)
{
    MWindow * window = MWindow::getInstance();

    // time
    unsigned int frequency = 60;
    unsigned int skipTicks = 1000 / frequency - 5;
    unsigned long previousFrame = 0;
    unsigned long startTick = window->getSystemTick();

    while(updateThreadRunning)
    {
        MSemaphoreWaitAndLock(&updateSemaphore);

        if(window->getFocus())
        {
            MSemaphoreUnlock(&updateSemaphore);

            // compute target tick
            unsigned long currentTick = window->getSystemTick();

            unsigned long tick = currentTick - startTick;
            unsigned long frame = (unsigned long)(tick * (frequency * 0.001f));

            // update elapsed time
            unsigned int i;
            unsigned int steps = (unsigned int)(frame - previousFrame);

            // don't wait too much
            if(steps >= (frequency/2))
            {
                MSemaphoreWaitAndLock(&updateSemaphore);

                    update();
                    unsigned int oldTick = currentTick;
                    currentTick = window->getSystemTick();

                MSemaphoreUnlock(&updateSemaphore);

                previousFrame = frame;

                MSleep(skipTicks - (currentTick - oldTick));
                continue;
            }

            MSemaphoreWaitAndLock(&updateSemaphore);

            // update
            for(i=0; i<steps; i++)
            {
                update();
                previousFrame++;
            }

            unsigned int oldTick = currentTick;
            currentTick = window->getSystemTick();

            MSemaphoreUnlock(&updateSemaphore);
            MSleep(skipTicks - (currentTick - oldTick));
        }
        else
        {
            MSemaphoreUnlock(&updateSemaphore);
            MSleep(100);
        }
    }

    return 0;
}

int input_thread(void* data)
{
    while(updateThreadRunning)
    {
        MWindow::getInstance()->onEvents();

        // flush input
        //MEngine::getInstance()->getInputContext()->flush();

        MSleep(10);
    }

    return 0;
}

// TODO: Make profiler a compiler option
// main
int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");

	unsigned int width = 1024;
	unsigned int height = 768;
    int fullscreen = false;
    int profiler = false;

    for(int i = 0; i < argc; i++)
    {
        printf("argc = %d, argv[argc] = %s\n", i, argv[i]);
    }

	if(argc > 2)
		sscanf(argv[2], "%d", &width);
	if(argc > 3)
		sscanf(argv[3], "%d", &height);
	if(argc > 4)
		sscanf(argv[4], "%d", &fullscreen);
    if(argc > 5)
        sscanf(argv[5], "%d", &profiler);
	
	// get engine (first time call onstructor)
	MEngine * engine = MEngine::getInstance();
	
	// get window (first time call onstructor)
	MWindow * window = MWindow::getInstance();

    // create window
	if(! window->create("Maratis", width, height, 32, fullscreen == 1))
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
			MProject embeddedProj;
			embeddedProj.renderer = s_embedded_renderer;

			char levelName[256];
			getGlobalFilename(levelName, window->getCurrentDirectory(), s_embedded_level_name);
			char projName[256];
			getGlobalFilename(projName, window->getCurrentDirectory(), s_embedded_game_name);

			embeddedProj.startLevel = levelName;
			maratis->loadProject(&embeddedProj, projName);
			engine->getGame()->begin();
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
						engine->getGame()->begin();
						projectFound = true;
						break;
					}
				}
			}
		}
	}
	
    if(profiler)
        MLOG_INFO("Profiling enabled");

    // create the update thread
    MThread updateThread;
    MThread inputThread;

    // Init semaphore
    updateSemaphore.Init(1);
    MInitSchedule();

    updateThread.Start(update_thread, "Update", NULL);
    inputThread.Start(input_thread, "Input", NULL);

    bool isActive = true;
    // on events
    while(isActive)
    {
        MSemaphoreWaitAndLock(&updateSemaphore);

        MUpdateScheduledEvents();

        if(!isActive)
        {
            engine->getGame()->end();
            MSemaphoreUnlock(&updateSemaphore);
            break;
        }

        if(window->getFocus())
        {
            int tick = engine->getSystemContext()->getSystemTick();
            draw();

            if(profiler)
            {
                printf("profiler frametime %d\n", engine->getSystemContext()->getSystemTick() - tick);
                fflush(stdout);
            }
        }
        else
        {
            draw();
            MSleep(100);
        }

        isActive = engine->isActive();

        // update postponed requests
        MEngine::getInstance()->updateRequests();

        MSemaphoreUnlock(&updateSemaphore);

        MSleep(0);
        //window->sleep(0.001); // 1 mili sec seems to slow down on some machines...
    }

    updateThreadRunning = false;
    int ret = updateThread.WaitForReturn();

    printf("Info: Update thread returned with exit code %d\n", ret);

	maratis->clear();
	return 0;
}

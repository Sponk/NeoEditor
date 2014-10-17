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
#include <SDL_log.h>

#include <MGameWinEvents.h>
#include "Maratis/MaratisPlayer.h"
#include <emscripten.h>

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

void complete_update()
{
	MEngine * engine = MEngine::getInstance();
	MWindow * window = MWindow::getInstance();
	
	// quit
	if(! engine->isActive())
	{
		MLOG_INFO("Engine is not active anymore!");
		//engine->getGame()->end();
		//break;
	}
	
	// on events
    window->onEvents();

    update();
    engine->getLevel()->getCurrentScene()->draw(engine->getLevel()->getCurrentScene()->getCurrentCamera());
    //draw();
    /*MLOG_INFO("Update");*/
    //MRenderingContext* render = engine->getRenderingContext();
    //render->setClearColor(MVector4(1.0,1.0,1.0,1.0));
    //render->clear(M_BUFFER_COLOR);

	//window->sleep(0.001); // 1 mili sec seems to slow down on some machines...
}

const char* asset_directory = "./project/";

// main
int main(int argc, char* argv[])
{
	unsigned int width = 1024;
	unsigned int height = 768;
	int fullscreen = false;
	
	if(argc > 2)
		sscanf(argv[2], "%d", &width);
	if(argc > 3)
		sscanf(argv[3], "%d", &height);
	if(argc > 4)
		sscanf(argv[4], "%d", &fullscreen);
	
	
	// get engine (first time call onstructor)
	MEngine * engine = MEngine::getInstance();
	
	
	// get window (first time call onstructor)
	MWindow * window = MWindow::getInstance();
	
	// create window
	if(!window->create("Maratis", width, height, 32, fullscreen == 1))
	{
		MLOG_ERROR("cannot create window");
		return -1;
	}
	
	if(fullscreen)
		window->hideCursor();
	
	
    window->setCurrentDirectory(".");
	
	// get Maratis (first time call onstructor)
	MaratisPlayer * maratis = MaratisPlayer::getInstance();
	
	// window pointer event
	window->setPointerEvent(windowEvents);

	// look for an mproj in the current directory
	std::vector<std::string> files;
	readDirectory(asset_directory, &files);
	
	bool found = false;
	
	if(files.size() == 0)
		MLOG_INFO("Did not find any file!");
	
	for(int i = 0; i < files.size(); ++i)
	{
        MLOG_INFO("Found " << files[i]);

		if(strstr(files[i].c_str(), ".mproj"))
		{
			if(maratis->loadProject((string(asset_directory) + files[i]).c_str()))
			{
				engine->getGame()->begin();
				found = true;
				break;
			}
		}
    }
	
	if(!found)
		MLOG_INFO("Did not find any project file");	
	
	emscripten_set_main_loop(complete_update, 60, 1);
	//maratis->clear();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
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


#include <vector>
#include <time.h>
#include <stdio.h>

#include <MEngine.h>
#include <MLoaders/MImageLoader.h>
#include <MLog.h>
#include "MSDLThread/MSDLThread.h"
#include "MFilesUpdate/MFilesUpdate.h"
#include "Maratis/Maratis.h"
#include "Maratis/MaratisUI.h"
#include <MCore.h>
#include <MSchedule/MSchedule.h>

#ifdef main
    #undef main
#endif

// logo
bool logo = true;
unsigned int logoTextureId = 0;
void drawLogo(void)
{
	MWindow * window = MWindow::getInstance();

	MGui2d quad;
	quad.setPosition(MVector2((window->getWidth()-512)*0.5f, (window->getHeight()-512)*0.5f));
	quad.setScale(MVector2(512, 512));
	quad.drawTexturedQuad(logoTextureId);
}

// update
void update(void)
{
	if(! logo)
		Maratis::getInstance()->logicLoop();
}

// draw
void draw(void)
{
	MMouse * mouse = MMouse::getInstance();
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	Maratis::getInstance()->graphicLoop();

	if(logo)
	{
		set2dMode(render);
		render->setBlendingMode(M_BLENDING_ALPHA);
		render->enableTexture();
		render->setColor4(MVector4(1.0f));
		drawLogo();
	}

	if(mouse->isLeftButtonPushed() || mouse->isMiddleButtonPushed() || (mouse->getWheelDirection() != 0))
		logo = false;

	MWindow::getInstance()->swapBuffer();
}

MSDLSemaphore updateSemaphore;
bool updateThreadRunning = true;

int update_thread(void* nothing)
{
    MWindow * window = MWindow::getInstance();
    MEngine * engine = MEngine::getInstance();
    Maratis * maratis = Maratis::getInstance();
    MGui * gui = MGui::getInstance();
    MaratisUI * UI = MaratisUI::getInstance();

    // time
    unsigned int frequency = 60;
    unsigned int skipTicks = 1000 / frequency - 5;
    unsigned long previousFrame = 0;
    unsigned long startTick = window->getSystemTick();

    int f = 0;
    int t = 0;

    while(updateThreadRunning)
    {
		MSDLSemaphore::WaitAndLock(&updateSemaphore);

            if(window->getFocus())
            {
				MSDLSemaphore::Unlock(&updateSemaphore);

                // compute target tick
                long currentTick = window->getSystemTick();

                long tick = currentTick - startTick;
                long frame = (long)(tick * (frequency * 0.001f));

                // update elapsed time
                int i;
                int steps = (int)(frame - previousFrame);

                // don't wait too much
                if(steps >= (frequency/2))
                {
					MSDLSemaphore::WaitAndLock(&updateSemaphore);

                        update();
                        unsigned int oldTick = currentTick;
                        currentTick = window->getSystemTick();

					MSDLSemaphore::Unlock(&updateSemaphore);

                    previousFrame = frame;

                    window->sleep(skipTicks - (currentTick - oldTick));
                    continue;
                }

				MSDLSemaphore::WaitAndLock(&updateSemaphore);

                // update
                for(i=0; i<steps; i++)
                {
                    update();
                    previousFrame++;
                    t++;
                    if(t == frequency)
                    {
                        MGame * game = engine->getGame();
                        if(game)
                        {
                            if(! game->isRunning())
                                MFilesUpdate::update();
                        }
                        else
                        {
                            MFilesUpdate::update();
                        }

                        //printf("fps:%d\n", f);
                        t = 0;
                        f = 0;
                    }
                }

            unsigned int oldTick = currentTick;
            currentTick = window->getSystemTick();

			MSDLSemaphore::Unlock(&updateSemaphore);

            window->sleep(skipTicks - (currentTick - oldTick));
        }
        else
        {
			MSDLSemaphore::Unlock(&updateSemaphore);
			window->sleep(100);
        }

        //window->sleep(0.001); // 1 mili sec seems to slow down on some machines...
    }

    return 0;
}

// main
int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");

	// get engine (first time call constructor)
	MEngine * engine = MEngine::getInstance();

	// get window (first time call constructor)
	MWindow * window = MWindow::getInstance();

    // create the update thread
    MSDLThread updateThread;

    // Init semaphore
    updateSemaphore.Init(1);

	// create window
	if(! window->create("Maratis", 1024,768, 32, false))
	{
		MLOG_ERROR("cannot create window");
		return 0;
	}

	// set current directory
	char rep[256];
	getRepertory(rep, argv[0]);
	window->setCurrentDirectory(rep);
	
	// init Maratis (first time call constructor)
	Maratis * maratis = Maratis::getInstance();
	
	// rendering context
	MRenderingContext * render = engine->getRenderingContext();
	MLOG_INFO("Render version : " << render->getRendererVersion());
	
	
	// init gui
	MGui * gui = MGui::getInstance();
	gui->setRenderingContext(render);
	gui->addFont(new MGuiTextureFont("font/default.tga"));

	
	// init MaratisUI
	MaratisUI * UI = MaratisUI::getInstance();
	window->setPointerEvent(MaratisUI::windowEvents);

	// logo
	{
		MImage image;
		if(! M_loadImage("gui/Title.png", &image))
		{
            MLOG_ERROR("cant load title image");
            return 0;
        }

		render->createTexture(&logoTextureId);
		render->bindTexture(logoTextureId);
		render->sendTextureImage(&image, false, false, false);

		// clear window
		draw();

		MGuiText text("LOADING", MVector2(480, 280), 16, MVector4(1, 1, 1, 1));
		text.draw();

		window->swapBuffer();
	}
	
	// load project
	if(argc > 1)
    {
		char filename[256];
		getGlobalFilename(filename, window->getCurrentDirectory(), argv[1]);
		maratis->loadProject(filename);
	}

    MInitSchedule();
    updateThread.Start(update_thread, "Update", NULL);

    bool isActive = true;
	// on events
    while(isActive)
	{
		MSDLSemaphore::WaitAndLock(&updateSemaphore);

		MEngine::getInstance()->getInputContext()->flush();
        window->onEvents();
        MUpdateScheduledEvents();

        if(!engine->isActive())
        {
            UI->endGame();
            engine->setActive(true);
        }

        if(window->getFocus())
        {
            draw();
        }
        else
        {
            draw();
            window->sleep(100);
        }

        // update postponed requests
        MEngine::getInstance()->updateRequests();

        isActive = window->isActive();
		MSDLSemaphore::Unlock(&updateSemaphore);

        window->sleep(0);
		//window->sleep(0.001); // 1 mili sec seems to slow down on some machines...
	}

    updateThreadRunning = false;
    int ret = updateThread.WaitForReturn();

	MLOG_INFO("Update thread returned with exit code " << ret);

	gui->clear();
	maratis->clear();
	return 0;
}

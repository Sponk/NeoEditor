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
#include "MFilesUpdate/MFilesUpdate.h"
#include "Maratis/Maratis.h"
#include "MWindow/MWindow.h"
#include <MCore.h>
#include <MSchedule/MSchedule.h>
#include "FLUID/MainWindow.h"
#include "FLUID/Callbacks.h"
#include <FL/Fl.H>

#include "MPluginScript/MPluginScript.h"

#ifdef main
    #undef main
#endif

extern void update_scene_tree();
extern void update_editor(void*);

Fl_Double_Window* main_window = NULL;
EditorWindow window;
const char* executable = NULL;

std::vector<MPluginScript*> editorPlugins;

void loadPluginsFrom(const char* src)
{
    std::vector<std::string> files;
    char dir[256];
    getGlobalFilename(dir, src, "plugins");

    readDirectory(dir, &files, false, false);
    MLOG_INFO("Searching for plugins in " << dir);

    std::string currentFile;
    for(int i = 0; i < files.size(); i++)
    {
        currentFile = files[i];

        // Check if file ends with ".lua"
        if(currentFile.find_last_of(".lua") == currentFile.length()-1)
        {
            MPluginScript* script = new MPluginScript;

            currentFile = dir + std::string("/") + currentFile;

            script->runScript(currentFile.c_str());
            editorPlugins.push_back(script);
        }
    }

    files.clear();
}

// main
int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");

    // set current directory
    char rep[256];
    getRepertory(rep, argv[0]);

    //Fl::background(46, 82, 115);

    /*Fl::foreground(255*0.32, 255*0.53, 255*0.7);
    Fl::background(0x26, 0x57, 0x7D);
    Fl::background2(255*0.32, 255*0.53, 255*0.70);*/

    /*Fl::foreground(44, 44, 42);
    Fl::background(44, 43, 39);
    Fl::background2(15, 15, 13);*/

    // gtk+ plastic none
    Fl::scheme("gleam");

    MWindow* mwindow = MWindow::getInstance();
    mwindow->setCurrentDirectory(rep);

    executable = argv[0];

    main_window = window.show_window();
    main_window->show();

    if(argc > 1)
        current_project.file_path = argv[1];

    // Init the engine
    MEngine * engine = MEngine::getInstance();
    Maratis * maratis = Maratis::getInstance();

    if(!current_project.file_path.empty())
    {
        current_project.changed = false;
        current_project.path = current_project.file_path;

#ifndef WIN32
        current_project.path = current_project.path.erase(current_project.path.find_last_of("/")+1, current_project.path.length());
#else
        current_project.path = current_project.path.erase(current_project.path.find_last_of("\\")+1, current_project.path.length());
#endif
        maratis->loadProject(current_project.file_path.c_str());
        current_project.level = maratis->getCurrentLevel();
    }

    update_scene_tree();

    // Load all plugins (TODO: Search in user home too!)
    loadPluginsFrom(rep);

    Fl::add_timeout(0.2, update_editor);
    Fl::run();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// main.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
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
#include <fstream>
#include "FLUID/ini.h"

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

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

extern void update_scene_tree();
extern void update_editor(void*);

Fl_Double_Window* main_window = NULL;
EditorWindow window;
const char* executable = NULL;

std::vector<MPluginScript*> editorPlugins;

bool vectorContains(std::string name)
{
	for(int i = 0; i < editorPlugins.size(); i++)
	{
		if(editorPlugins[i]->getName() == name)
			return true;
	}

	return false;
}

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
		if (currentFile.find_last_of(".lua") == currentFile.length() - 1)
		{
			MPluginScript* script = new MPluginScript;

			currentFile = dir + std::string("/") + currentFile;

			script->runScript(currentFile.c_str());

			if (!vectorContains(script->getName()))
			{
				editorPlugins.push_back(script);
			}
			else
			{
				MLOG_ERROR("Multiple plugins with the same name loaded:\n\tName: " << script->getName() << "\n\tFile: " << currentFile);

				fl_message("There are multiple plugins with the same name inside the plugin search path.\n"
							"Remove all redundant scripts or else the editor will not be able to start anymore.\n\n"
							"Name: %s\nFile: %s\n ", script->getName().c_str(), currentFile.c_str());

				MLOG_ERROR("Exiting editor due to errors!");
				exit(-1);
			}
		}
    }

    files.clear();
	MLOG_INFO("Successfully loaded " << editorPlugins.size() << " plugins.");
}

#define STR1_TO_FLOAT(x) static_cast<float>(::atof(x.c_str()))

void loadSettings(const char* path)
{
    std::string fullpath = path;

#ifndef WIN32
    fullpath += "/.neoeditor/";
#else
    fullpath += "\\neoeditor\\";
#endif

    char dir[256];
    getGlobalFilename(dir, fullpath.c_str(), "config.ini");

    if(!isFileExist(dir))
    {
        MLOG_INFO("Config does not exist. Using defaults.");
        return;
    }

    MLOG_INFO("Loading config: " << dir);
    INI::Parser parser(dir);

    MVector3 vector(STR1_TO_FLOAT(parser.top()("theme")["background_r"]), STR1_TO_FLOAT(parser.top()("theme")["background_g"]), STR1_TO_FLOAT(parser.top()("theme")["background_b"]));
    vector *= 255.0f;

    Fl::background(static_cast<unsigned char>(vector.x), static_cast<unsigned char>(vector.y), static_cast<unsigned char>(vector.z));

    vector = MVector3(STR1_TO_FLOAT(parser.top()("theme")["background2_r"]), STR1_TO_FLOAT(parser.top()("theme")["background2_g"]), STR1_TO_FLOAT(parser.top()("theme")["background2_b"]));
    vector *= 255.0f;

    Fl::background2(static_cast<unsigned char>(vector.x), static_cast<unsigned char>(vector.y), static_cast<unsigned char>(vector.z));

    vector = MVector3(STR1_TO_FLOAT(parser.top()("theme")["foreground_r"]), STR1_TO_FLOAT(parser.top()("theme")["foreground_g"]), STR1_TO_FLOAT(parser.top()("theme")["foreground_b"]));
    vector *= 255.0f;

    Fl::foreground(static_cast<unsigned char>(vector.x), static_cast<unsigned char>(vector.y), static_cast<unsigned char>(vector.z));
    Fl::scheme(parser.top()("theme")["scheme"].c_str());

    window.inputMethod = NULL;
    std::string inputMethod = parser.top()("input")["inputMethod"];
    for(int i = 0; i < editorPlugins.size(); i++)
    {
        if(editorPlugins[i]->getName() == inputMethod)
        {
            window.inputMethod = editorPlugins[i];
        }
    }

    int width = STR1_TO_FLOAT(parser.top()("window")["width"]);
    int height = STR1_TO_FLOAT(parser.top()("window")["height"]);

    // Default window size
    if(width == 0)
        width = 1024;

    if(height == 0)
        height = 768;

    main_window->resize(STR1_TO_FLOAT(parser.top()("window")["xpos"]), STR1_TO_FLOAT(parser.top()("window")["ypos"]), width, height);

    translation_speed = STR1_TO_FLOAT(parser.top()("window")["translationSpeed"]);
    rotation_speed = STR1_TO_FLOAT(parser.top()("window")["rotationSpeed"]);

    // Default values
    if(rotation_speed == 0.0)
        rotation_speed = 1.0;

    if(translation_speed == 0.0)
        translation_speed = 1.0;

    window.translation_speed->value(translation_speed);
    window.rotation_speed->value(rotation_speed);
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

    // gtk+ plastic none gleam
    Fl::scheme("gtk+");
    Fl::get_system_colors();

    MWindow* mwindow = MWindow::getInstance();
    mwindow->setCurrentDirectory(rep);

    executable = argv[0];

    main_window = window.show_window();
    main_window->xclass("NeoEditor");
    main_window->show();

    if(argc > 1)
        current_project.file_path = argv[1];

    // Init the engine
    MEngine * engine = MEngine::getInstance();
    Maratis * maratis = Maratis::getInstance();

    engine->setGame(NULL);

    // Load all plugins (TODO: Search in user home too!)
    loadPluginsFrom(rep);

#ifndef WIN32
    loadSettings(getenv("HOME"));
#else
    loadSettings(getenv("APPDATA"));
#endif

    Fl::add_timeout(0.2, update_editor);
    Fl::run();
	return 0;
}

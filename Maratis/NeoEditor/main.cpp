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
#include <signal.h>
#include <stdio.h>
#include <fstream>
#include "FLUID/ini.h"

#ifdef __GNUC__
    #include <execinfo.h>
    #include <cxxabi.h>
#endif

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

// Based on stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0
std::string stacktrace(unsigned int max_frames = 63)
{
    std::string output = "Stacktraces are not available for your architecture!";

    // Stacktrace for x86/64 GCC versions
#if !defined(__arm__) &&  !defined(_MSC_VER)

    output = "Stacktrace:\n\n";

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0)
    {
        output.append("  <empty, possibly corrupt>\n");
        return output;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++)
    {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbollist[i]; *p; ++p)
        {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
            break;
            }
        }

        if (begin_name && begin_offset && end_offset
            && begin_name < begin_offset)
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
                            funcname, &funcnamesize, &status);

            if (status == 0)
            {
                funcname = ret; // use possibly realloc()-ed string
                output.append(std::string(symbollist[i]) + " : " + funcname + "+" + begin_offset + "\n");
            }
            else
            {
            // demangling failed. Output function name as a C function with
            // no arguments.
                output.append(std::string(symbollist[i]) + " : " + funcname + "+" + begin_offset + "\n");
            }
        }
        else
        {
            // couldn't parse the line? print the whole line.
            output.append(std::string(symbollist[i]) + "\n");
        }
    }

    free(funcname);
    free(symbollist);
#endif
    return output;
}

void close_crash_handler(Fl_Button*, void*)
{
    exit(-1);
}

void crash_handler(int sig)
{
    switch(sig)
    {
    case SIGFPE:
    case SIGILL:
    case SIGSEGV:
        {
            CrashHandlerDlg dlg;
            Fl_Window* win = dlg.create_window();
            win->show();

            std::string stack = stacktrace();
            MLOG_ERROR("Stack trace: " << endl << stack);

            Fl_Text_Buffer buffer;
            buffer.text(stack.c_str());
            dlg.stack_output->buffer(&buffer);

            while(win->shown())
                Fl::wait();

            exit(-1);
            break;
        }
    }
}

// main
int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");

    // Set crash handler
    signal(SIGFPE,  crash_handler);
    signal(SIGILL,  crash_handler);
    signal(SIGSEGV, crash_handler);

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

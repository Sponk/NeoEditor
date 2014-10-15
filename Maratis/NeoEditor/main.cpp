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
#include <iterator>
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
#include "FLUID/Translator.h"

#ifdef _WIN32
    #include <windows.h>
    #include <imagehlp.h>
#endif

#if defined(__GNUC__) && !defined(_WIN32)
    #include <execinfo.h>
    #include <cxxabi.h>
#endif

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
#if !defined(__arm__) &&  !defined(_MSC_VER) && !defined(_WIN32)

    output = "Stacktrace:\n\n";

    void* addrlist[max_frames+1];
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0)
    {
        output.append("  <empty, possibly corrupt>\n");
        return output;
    }

    char** symbollist = backtrace_symbols(addrlist, addrlen);
    std::string symbol;
    std::string function;
    std::string tmp;
    size_t size = 255;

    char demangled[255];

    int status;

    // Ignore first three stackframes since they only contain
    // Error handling
    for(int i = 3; i < addrlen; i++)
    {
        symbol = symbollist[i];
        symbol = symbol.substr(symbol.find_last_of("/")+1);

        int idx = symbol.find("(")+1;
        tmp = function = symbol.substr(idx, symbol.find_first_of('+')-idx);

        abi::__cxa_demangle(function.c_str(), demangled, &size, &status);
        function = demangled;

        switch(status)
        {
        case 0:
            {
                int idx = symbol.find(tmp);
                symbol.replace(idx, tmp.length(), "");
                symbol.insert(idx, function);
            }
            break;

        default:
            break;
        }

        output.append(symbol);
        output.push_back('\n');
    }
#endif
    return output;
}

#ifndef _WIN32
void crash_handler(int sig)
{
    std::string signal;
    switch(sig)
    {
    case SIGFPE:
        signal = "SIGFPE";
    case SIGILL:
        if(signal.empty())
            signal = "SIGFPE";
    case SIGSEGV:
        if(signal.empty())
            signal = "SIGSEGV";

        {
            char signum[3];
            snprintf(signum, 3, "%d", sig);
            std::string stack = stacktrace();
            std::string complete_text = "Catched signal " + string(signum) + " (" + signal + ")\n";

            complete_text += "\nSystem data:\n";

            FILE* pipe = popen("uname -p -o -r", "r");
            char c;
            while((c = getc(pipe)) != EOF)
            {
                complete_text.push_back(c);
            }

            complete_text.append("\n");
            complete_text.append(stack);

            MLOG_ERROR(complete_text << endl);

            std::string path = getenv("HOME");
            path += "/.neoeditor/logfile.txt";

            copyFile("logfile.txt", path.c_str());
            system("./CrashHandler");

            exit(sig);
            break;
        }
    }
}
#endif

#ifdef _WIN32

std::string win32_stacktrace(CONTEXT* context)
{
    SymInitialize(GetCurrentProcess(), 0, true);
    std::string output;

    STACKFRAME frame = { 0 };

    frame.AddrPC.Offset         = context->Eip;
    frame.AddrPC.Mode           = AddrModeFlat;
    frame.AddrStack.Offset      = context->Esp;
    frame.AddrStack.Mode        = AddrModeFlat;
    frame.AddrFrame.Offset      = context->Ebp;
    frame.AddrFrame.Mode        = AddrModeFlat;

    char* symbolBuffer = new char[sizeof(IMAGEHLP_SYMBOL) + 255];
    memset(symbolBuffer, 0, sizeof(IMAGEHLP_SYMBOL) + 255);

    IMAGEHLP_SYMBOL* symbol = (IMAGEHLP_SYMBOL*) symbolBuffer;
    symbol->SizeOfStruct    = sizeof(IMAGEHLP_SYMBOL) + 255;
    symbol->MaxNameLength   = 254;

    unsigned int displacement = 0;

    char num[32];
    while (StackWalk(IMAGE_FILE_MACHINE_I386 ,
                     GetCurrentProcess(),
                     GetCurrentThread(),
                     &frame,
                     context,
                     0,
                     SymFunctionTableAccess,
                     SymGetModuleBase,
                     0 ) )
    {
        sprintf(num, "0x%x", frame.AddrPC.Offset);

        if(SymGetSymFromAddr(GetCurrentProcess(), frame.AddrPC.Offset, (DWORD*) &displacement, symbol))
        {
            output += symbol->Name;
            output += " [";
            output += num;
            output += "]\n";
        }
        else
        {
            output.append(string(num) + "\n");
        }
    }

    delete symbolBuffer;
    SymCleanup(GetCurrentProcess());
    return output;
}

LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS * ExceptionInfo)
{
  switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
  {
    case EXCEPTION_ACCESS_VIOLATION:
      MLOG_ERROR("EXCEPTION_ACCESS_VIOLATION");
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      MLOG_ERROR("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
      break;
    case EXCEPTION_BREAKPOINT:
      MLOG_ERROR("EXCEPTION_BREAKPOINT");
      break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
      MLOG_ERROR("EXCEPTION_DATATYPE_MISALIGNMENT");
      break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
      MLOG_ERROR("EXCEPTION_FLT_DENORMAL_OPERAND");
      break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      MLOG_ERROR("EXCEPTION_FLT_DIVIDE_BY_ZERO");
      break;
    case EXCEPTION_FLT_INEXACT_RESULT:
      MLOG_ERROR("EXCEPTION_FLT_INEXACT_RESULT");
      break;
    case EXCEPTION_FLT_INVALID_OPERATION:
      MLOG_ERROR("EXCEPTION_FLT_INVALID_OPERATION");
      break;
    case EXCEPTION_FLT_OVERFLOW:
      MLOG_ERROR("EXCEPTION_FLT_OVERFLOW");
      break;
    case EXCEPTION_FLT_STACK_CHECK:
      MLOG_ERROR("EXCEPTION_FLT_STACK_CHECK");
      break;
    case EXCEPTION_FLT_UNDERFLOW:
      MLOG_ERROR("EXCEPTION_FLT_UNDERFLOW");
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      MLOG_ERROR("EXCEPTION_ILLEGAL_INSTRUCTION");
      break;
    case EXCEPTION_IN_PAGE_ERROR:
      MLOG_ERROR("EXCEPTION_IN_PAGE_ERROR");
      break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      MLOG_ERROR("EXCEPTION_INT_DIVIDE_BY_ZERO");
      break;
    case EXCEPTION_INT_OVERFLOW:
      MLOG_ERROR("EXCEPTION_INT_OVERFLOW");
      break;
    case EXCEPTION_INVALID_DISPOSITION:
      MLOG_ERROR("EXCEPTION_INVALID_DISPOSITION");
      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      MLOG_ERROR("EXCEPTION_NONCONTINUABLE_EXCEPTION");
      break;
    case EXCEPTION_PRIV_INSTRUCTION:
      MLOG_ERROR("EXCEPTION_PRIV_INSTRUCTION");
      break;
    case EXCEPTION_SINGLE_STEP:
      MLOG_ERROR("EXCEPTION_SINGLE_STEP");
      break;
    case EXCEPTION_STACK_OVERFLOW:
      MLOG_ERROR("EXCEPTION_STACK_OVERFLOW");
      break;
    default:
      MLOG_ERROR("Unrecognized Exception");
      break;
  }

  std::string stack;
  if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode)
  {
      stack = win32_stacktrace(ExceptionInfo->ContextRecord);
  }

  char signum[3];
  snprintf(signum, 3, "%d", ExceptionInfo->ExceptionRecord->ExceptionCode);
  std::string complete_text = "Catched signal " + string(signum) + "\n";

  complete_text += "\nSystem data:\n";

  const char* env = getenv("OS");
  complete_text.append(env);
  complete_text.push_back(' ');

  env = getenv("PROCESSOR_ARCHITECTURE");
  complete_text.append(env);

  complete_text.append("\n\n");
  complete_text.append(stack);

  MLOG_ERROR(complete_text << endl);

  std::string path = getenv("APPDATA");
  path += "\\neoeditor\\logfile.txt";

  copyFile("logfile.txt", path.c_str());
  system(".\\CrashHandler.exe");

  exit(-1);
}
#endif

void load_translation(const char* confdir, const char* rep)
{
    std::string fullpath = confdir;

#ifndef WIN32
    fullpath += "/.neoeditor/";
#else
    fullpath += "\\neoeditor\\";
#endif

    char dir[256];
    getGlobalFilename(dir, fullpath.c_str(), "language.ini");

    INI::Parser parser(dir);
    string langname = parser.top()("lang")["name"].c_str();

    // Default to english
    if(langname.empty())
        langname = "english.ini";

#ifndef WIN32
    langname = string("translations/") + langname;
#else
    langname = string("translations\\") + langname;
#endif

    getGlobalFilename(dir, rep, langname.c_str());
    Translator::getInstance()->loadTranslation(dir);
}

// main
int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");

    // set current directory
    char rep[256];
    getRepertory(rep, argv[0]);

#ifndef WIN32
    load_translation(getenv("HOME"), rep);
#else
    load_translation(getenv("APPDATA"), rep);
#endif

    // Set crash handler
#ifndef _WIN32
    signal(SIGFPE,  crash_handler);
    signal(SIGILL,  crash_handler);
    signal(SIGSEGV, crash_handler);
#else
    SetUnhandledExceptionFilter(windows_exception_handler);
#endif

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

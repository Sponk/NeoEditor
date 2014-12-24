#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include "MainWindow.h"
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

string currentDirectory;

// FLTK helper
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type)
{
	Fl_Native_File_Chooser* dlg = new Fl_Native_File_Chooser();
	dlg->title(title);
	dlg->filter(files);
	dlg->type(type);
	dlg->directory(dir);

	if (dlg->show() == 0)
		return dlg->filename();

	return NULL;
}

int main(int argc, char **argv)
{
    Fl::get_system_colors();
    Fl::scheme("gtk+");

	currentDirectory = argv[0];
#ifndef WIN32
	currentDirectory = currentDirectory.erase(currentDirectory.find_last_of("/"));
#else
	currentDirectory = currentDirectory.erase(currentDirectory.find_last_of("\\"));
#endif

    MainWindow dlg;
    Fl_Double_Window* window = dlg.create_window();

	dlg.loadProjectList();
	dlg.update_package_list();

    window->show();
    return Fl::run();
}

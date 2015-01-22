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

int main(int argc, char **argv)
{
	Fl::background(0xEE,0xEE,0xEE);
	Fl::background2(0xFF,0xFF,0xFF);

	//Fl::get_system_colors();
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

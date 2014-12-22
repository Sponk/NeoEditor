#include <FL/Fl.H>
#include "MainWindow.h"
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char **argv)
{
    Fl::get_system_colors();
    Fl::scheme("gtk+");

    MainWindow dlg;
    Fl_Double_Window* window = dlg.create_window();


    window->show();
    return Fl::run();
}

#include <FL/Fl.H>
#include "CrashHandler.h"
#include <string>

int main(int argc, char **argv)
{
    Fl::get_system_colors();
    Fl::scheme("gtk+");

    CrashHandlerDlg dlg;
    Fl_Double_Window* window = dlg.create_window();

    Fl_Text_Buffer buffer;
    dlg.stack_output->buffer(&buffer);

#ifndef _WIN32
    std::string path = getenv("HOME");
    path += "/.neoeditor/logfile.txt";

    buffer.loadfile(path.c_str());
#else
    std::string path = getenv("APPDATA");
    path += "\\neoeditor\\logfile.txt";

    buffer.loadfile(path.c_str());
#endif
    window->show();
    return Fl::run();
}

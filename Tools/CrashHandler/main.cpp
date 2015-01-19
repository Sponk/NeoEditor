#include <FL/Fl.H>
#include "CrashHandler.h"
#include <string>
#include <FL/fl_message.H>

#include <HTTPRequest.h>

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#ifndef REPORT_DOMAIN
#define REPORT_DOMAIN "127.0.0.1"
#endif

Fl_Text_Buffer buffer;

void send_report(Fl_Button* button, void*)
{
	Neo::HTTPRequest connection(REPORT_DOMAIN, 80);

	const char* response = connection.sendPostRequest("/crash.php", buffer.text());
}

int main(int argc, char **argv)
{
    //Fl::get_system_colors();

	Fl::background(0xEE,0xEE,0xEE);
	Fl::background2(0xFF,0xFF,0xFF);

	Fl::scheme("gtk+");

    CrashHandlerDlg dlg;
    Fl_Double_Window* window = dlg.create_window();

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

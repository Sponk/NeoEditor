#include <HTTPConnection.h>
#include <PacketizedTCP.h>
#include <RakSleep.h>

#include <FL/Fl.H>
#include "CrashHandler.h"
#include <string>
#include <FL/fl_message.H>

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
    RakNet::TCPInterface tcp;
    button->deactivate();

    RakNet::HTTPConnection connection;
    connection.Init(&tcp, REPORT_DOMAIN, 80);

    if(!tcp.Start(4000, 1))
    {
        fl_message("Could not connect to server!");
        button->activate();
        return;
    }

    connection.Post("/crash.php", buffer.text(), "text/html");
    //connection.Get("/index.html");

    while(1)
    {
        RakNet::Packet *packet = tcp.Receive();
        if(packet)
        {
            connection.ProcessTCPPacket(packet);
            tcp.DeallocatePacket(packet);
        }

        connection.Update();

        if (connection.IsBusy() == false)
        {
            RakNet::RakString fileContents = connection.Read();

            if(fileContents.Find("Sent crash report.") != -1)
            {
                fl_message("Successfully sent crash report.");
                exit(0);
            }
            else
            {
                fl_message("Could not send crash report!");
                button->activate();
                return;
            }
        }

        // Prevent 100% cpu usage
        RakSleep(30);
    }
}

int main(int argc, char **argv)
{
    Fl::get_system_colors();
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

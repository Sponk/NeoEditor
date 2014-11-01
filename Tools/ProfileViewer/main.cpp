#include <FL/Fl.H>
#include "MainWindow.h"
#include <fstream>
#include <string>
#include <ctime>
#include <FL/fl_message.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Chart.H>

using namespace std;

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char **argv)
{
    srand(time(0));

    const char* profile_path = NULL;
    if(argc > 1)
        profile_path = argv[1];

    Fl::get_system_colors();
    Fl::scheme("gtk+");

    MainWindow dlg;
    Fl_Double_Window* window = dlg.create_window();

    // Load the profile
    ifstream in(profile_path);
    if(!in)
    {
        fl_message("Could not load specified profile!");
        return EXIT_FAILURE;
    }

    string line;
    string data;

    int x = 1, y = 0;
    
    std::getline(in, line, '\n');
    std::getline(in, line, '\n');
    
    while(!in.eof())
    {
        in >> line;

        // FIXME: Very hacky!
        if(line == "call")
        {
            in >> line;
            if(line == "tree")
                break;
        }

        Fl_Input* widget = (Fl_Input*) dlg.table->cell(x, y);

        if(y+1 == 3 || y+1 == 5)
        {
            in >> data;
            line += data;
        }

        y++;
        if(y > 6)
        {
            y = 1;
            x++;
        }

        widget->value(line.c_str());
    }

    dlg.chart->type(FL_PIE_CHART);
    dlg.time_chart->type(FL_BAR_CHART);

    float value = 0.0;
    Fl_Input* input = NULL;
    Fl_Input* value_input = NULL;

    // Set up pie chart
    for(int i = 1; i < x; i++)
    {
        input = (Fl_Input*) dlg.table->cell(i, 0);
        value_input = (Fl_Input*) dlg.table->cell(i, 3);

        sscanf(value_input->value(), "%f%%", &value);
        dlg.chart->add(value, input->value(), rand());
    }

    // Enough to save every possible unit
    char unit[5];
    // Set up bar chart
    for(int i = 1; i < x; i++)
    {
        input = (Fl_Input*) dlg.table->cell(i, 0);
        value_input = (Fl_Input*) dlg.table->cell(i, 2);

        sscanf(value_input->value(), "%f%s", &value, unit);

        // Convert all units to us
        if(!strcmp(unit, "us"))
            value *= 0.001;
        else if(!strcmp(unit, "s"))
            value *= 1000;
        else if(!strcmp(unit, "ns"))
            value *= 0.000001;

        dlg.time_chart->add(value, input->value(), rand());
    }

    in.close();
    window->show();
    return Fl::run();
}

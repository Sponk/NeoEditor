#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <FL/Fl.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <string>

typedef struct
{
    std::string path;
    std::string level;
    std::string file_path;
    bool changed;
}open_project_t;

extern open_project_t current_project;

extern float rotation_speed;
extern float translation_speed;

extern bool reload_editor;
extern bool update_name;

void edit_text_properties(Fl_Widget*, void*);
void update_behavior_menu();
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);

#endif

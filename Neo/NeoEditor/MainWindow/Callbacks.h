//========================================================================
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <FL/Fl.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_message.H>
#include <Window/Window.h>
#include <string>

// Maratis includes (for MainWindow.cxx)
#include "../Maratis/Maratis.h"
#include "../MainWindow/ini.h"
#include "../Publish/Publisher.h"

// Is now given via compiler flags
// #define EDITOR_VERSION_STRING "0.1"

typedef struct
{
    std::string path;
    std::string level;
    std::string file_path;
    bool changed;
}open_project_t;

extern open_project_t current_project;
extern Fl_Double_Window* main_window;

extern float rotation_speed;
extern float translation_speed;

extern bool reload_editor;
extern bool update_name;

void edit_text_properties(Fl_Widget*, void*);
void update_behavior_menu();
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);
void update_scene_tree();
void create_behavior_ui(Neo::Object3d* object);
void save_settings();

#endif

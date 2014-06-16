// generated by Fast Light User Interface Designer (fluid) version 1.0302

#include "MainWindow.h"
#include "Callbacks.h"

Fl_Menu_Item EditorWindow::menu_menu_bar[] = {
 {"File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"New", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Project", 0x5006e,  (Fl_Callback*)new_project_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Level", 0x4006e,  (Fl_Callback*)new_level_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Open", 0,  0, 0, 192, FL_NORMAL_LABEL, 0, 14, 0},
 {"Project", 0x5006f,  (Fl_Callback*)open_project_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Level", 0x4006f,  (Fl_Callback*)open_level_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Import Mesh", 0,  (Fl_Callback*)import_mesh_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save level", 0x40073,  (Fl_Callback*)save_level_callback, (void*)(0), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save level as...", 0x50073,  (Fl_Callback*)save_level_callback, (void*)(1), 128, FL_NORMAL_LABEL, 0, 14, 0},
 {"Quit", 0x40071,  (Fl_Callback*)quit_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Edit", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Undo", 0x4007a,  (Fl_Callback*)undo_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Redo", 0x5007a,  (Fl_Callback*)redo_callback, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {"Select all", 0x40061,  (Fl_Callback*)select_all_callback, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {"Duplicate object", 0x40064,  (Fl_Callback*)duplicate_object_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Delete object", 0xffff,  (Fl_Callback*)delete_object_callback, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {"Scene setup", 0,  (Fl_Callback*)scene_setup_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Delete current scene", 0,  (Fl_Callback*)delete_scene_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Project", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Publish", 0x40070,  (Fl_Callback*)publish_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Run Game", 0xffc2,  (Fl_Callback*)play_game_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Add", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Object", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Mesh", 0x4006d,  (Fl_Callback*)add_mesh_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Light", 0x4006c,  (Fl_Callback*)add_light_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Text", 0x40074,  (Fl_Callback*)add_text_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Camera", 0x40063,  (Fl_Callback*)add_camera_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Sound", 0,  (Fl_Callback*)add_sound_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Add Behavior", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Add Scene", 0,  (Fl_Callback*)add_scene_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Help", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"About", 0,  (Fl_Callback*)about_menu_callback, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};
Fl_Menu_Item* EditorWindow::behavior_menu = EditorWindow::menu_menu_bar + 35;

Fl_Menu_Item EditorWindow::menu_object_shape_choice[] = {
 {"Box", 0,  (Fl_Callback*)edit_shape_callback, (void*)(0), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Sphere", 0,  (Fl_Callback*)edit_shape_callback, (void*)(1), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Cone", 0,  (Fl_Callback*)edit_shape_callback, (void*)(2), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Capsule", 0,  (Fl_Callback*)edit_shape_callback, (void*)(3), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Cylinder", 0,  (Fl_Callback*)edit_shape_callback, (void*)(4), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Convex-hull", 0,  (Fl_Callback*)edit_shape_callback, (void*)(5), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Triangle-mesh", 0,  (Fl_Callback*)edit_shape_callback, (void*)(6), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item EditorWindow::menu_text_alignment_chooser[] = {
 {"Left", 0,  (Fl_Callback*)text_alignment_callback, (void*)(0), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Right", 0,  (Fl_Callback*)text_alignment_callback, (void*)(1), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Center", 0,  (Fl_Callback*)text_alignment_callback, (void*)(2), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Double_Window* EditorWindow::show_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(918, 624, "Maratis Editor");
    w = o;
    o->labelsize(11);
    o->user_data((void*)(this));
    { menu_bar = new Fl_Menu_Bar(0, 0, 920, 25);
      menu_bar->selection_color((Fl_Color)38);
      menu_bar->menu(menu_menu_bar);
    } // Fl_Menu_Bar* menu_bar
    { glbox = new GLBox(201, 56, 492, 569, "OpenGL box");
      glbox->box(FL_NO_BOX);
      glbox->color((Fl_Color)179);
      glbox->selection_color(FL_BACKGROUND_COLOR);
      glbox->labeltype(FL_NORMAL_LABEL);
      glbox->labelfont(0);
      glbox->labelsize(14);
      glbox->labelcolor(FL_FOREGROUND_COLOR);
      glbox->align(Fl_Align(FL_ALIGN_CENTER));
      glbox->when(FL_WHEN_RELEASE);
      Fl_Group::current()->resizable(glbox);
    } // GLBox* glbox
    { scene_tree = new Fl_Tree(0, 55, 201, 570);
      scene_tree->callback((Fl_Callback*)scene_tree_callback);
      scene_tree->when(FL_WHEN_RELEASE);
    } // Fl_Tree* scene_tree
    { edit_group = new Fl_Group(3, 24, 708, 30);
      { Fl_Round_Button* o = new Fl_Round_Button(258, 27, 90, 24, "Translate");
        o->type(102);
        o->shortcut(0x31);
        o->down_box(FL_ROUND_DOWN_BOX);
        o->value(1);
        o->callback((Fl_Callback*)set_edit_type, (void*)('t'));
      } // Fl_Round_Button* o
      { Fl_Round_Button* o = new Fl_Round_Button(345, 27, 69, 24, "Rotate");
        o->type(102);
        o->shortcut(0x32);
        o->down_box(FL_ROUND_DOWN_BOX);
        o->callback((Fl_Callback*)set_edit_type, (void*)('r'));
      } // Fl_Round_Button* o
      { Fl_Round_Button* o = new Fl_Round_Button(414, 27, 60, 24, "Scale");
        o->type(102);
        o->shortcut(0x33);
        o->down_box(FL_ROUND_DOWN_BOX);
        o->callback((Fl_Callback*)set_edit_type, (void*)('s'));
      } // Fl_Round_Button* o
      { scenes_menu = new Fl_Choice(564, 30, 147, 18, "Scenes:");
        scenes_menu->down_box(FL_BORDER_BOX);
      } // Fl_Choice* scenes_menu
      { speed_group = new Fl_Group(3, 27, 258, 21);
        { rotation_speed = new Fl_Value_Input(72, 27, 45, 20, "Rotation:");
          rotation_speed->value(1);
          rotation_speed->callback((Fl_Callback*)rotation_speed_callback);
          rotation_speed->align(Fl_Align(68));
          rotation_speed->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* rotation_speed
        { translation_speed = new Fl_Value_Input(207, 28, 45, 20, "Translation:");
          translation_speed->value(1);
          translation_speed->callback((Fl_Callback*)translation_speed_callback);
          translation_speed->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* translation_speed
        speed_group->resizable(NULL);
        speed_group->end();
      } // Fl_Group* speed_group
      edit_group->resizable(NULL);
      edit_group->end();
    } // Fl_Group* edit_group
    { Fl_Tabs* o = new Fl_Tabs(693, 54, 261, 645);
      { Fl_Tabs* o = new Fl_Tabs(693, 75, 261, 624, "Object");
        { Fl_Scroll* o = new Fl_Scroll(693, 75, 261, 624);
          { name_edit = new Fl_Input(699, 102, 171, 21, "Name:");
            name_edit->callback((Fl_Callback*)edit_name_callback);
            name_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            name_edit->when(FL_WHEN_ENTER_KEY);
          } // Fl_Input* name_edit
          { Fl_Group* o = new Fl_Group(696, 195, 222, 33, "Position:");
            o->box(FL_ENGRAVED_FRAME);
            o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            { xpos_edit = new Fl_Value_Input(720, 201, 51, 21, "X:");
              xpos_edit->maximum(1000);
              xpos_edit->step(1);
              xpos_edit->callback((Fl_Callback*)edit_object_callback, (void*)('t'));
              xpos_edit->soft(true);
            } // Fl_Value_Input* xpos_edit
            { ypos_edit = new Fl_Value_Input(792, 201, 48, 21, "Y:");
              ypos_edit->maximum(1000);
              ypos_edit->step(1);
              ypos_edit->callback((Fl_Callback*)edit_object_callback, (void*)('t'));
              ypos_edit->soft(true);
            } // Fl_Value_Input* ypos_edit
            { zpos_edit = new Fl_Value_Input(861, 201, 51, 21, "Z:");
              zpos_edit->maximum(1000);
              zpos_edit->step(1);
              zpos_edit->callback((Fl_Callback*)edit_object_callback, (void*)('t'));
              zpos_edit->soft(true);
            } // Fl_Value_Input* zpos_edit
            o->end();
          } // Fl_Group* o
          { Fl_Group* o = new Fl_Group(696, 255, 222, 33, "Rotation:");
            o->box(FL_ENGRAVED_FRAME);
            o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            { xrot_edit = new Fl_Value_Input(717, 261, 48, 21, "X:");
              xrot_edit->maximum(360);
              xrot_edit->step(5);
              xrot_edit->callback((Fl_Callback*)edit_object_callback, (void*)('r'));
              xrot_edit->soft(true);
            } // Fl_Value_Input* xrot_edit
            { yrot_edit = new Fl_Value_Input(786, 261, 48, 21, "Y:");
              yrot_edit->maximum(360);
              yrot_edit->step(5);
              yrot_edit->callback((Fl_Callback*)edit_object_callback, (void*)('r'));
              yrot_edit->soft(true);
            } // Fl_Value_Input* yrot_edit
            { zrot_edit = new Fl_Value_Input(858, 261, 48, 21, "Z:");
              zrot_edit->maximum(360);
              zrot_edit->step(5);
              zrot_edit->callback((Fl_Callback*)edit_object_callback, (void*)('r'));
              zrot_edit->soft(true);
            } // Fl_Value_Input* zrot_edit
            o->end();
          } // Fl_Group* o
          { Fl_Group* o = new Fl_Group(696, 312, 222, 33, "Scale:");
            o->box(FL_ENGRAVED_FRAME);
            o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            { xscale_edit = new Fl_Value_Input(717, 318, 48, 21, "X:");
              xscale_edit->maximum(20);
              xscale_edit->step(0.1);
              xscale_edit->value(1);
              xscale_edit->callback((Fl_Callback*)edit_object_callback, (void*)('s'));
              xscale_edit->soft(true);
            } // Fl_Value_Input* xscale_edit
            { yscale_edit = new Fl_Value_Input(786, 318, 48, 21, "Y:");
              yscale_edit->maximum(20);
              yscale_edit->step(0.1);
              yscale_edit->value(1);
              yscale_edit->callback((Fl_Callback*)edit_object_callback, (void*)('s'));
              yscale_edit->soft(true);
            } // Fl_Value_Input* yscale_edit
            { zscale_edit = new Fl_Value_Input(858, 318, 48, 21, "Z:");
              zscale_edit->maximum(20);
              zscale_edit->step(0.1);
              zscale_edit->value(1);
              zscale_edit->callback((Fl_Callback*)edit_object_callback, (void*)('s'));
              zscale_edit->soft(true);
            } // Fl_Value_Input* zscale_edit
            o->end();
          } // Fl_Group* o
          { parent_edit = new Fl_Input(699, 147, 171, 21, "Parent:");
            parent_edit->callback((Fl_Callback*)edit_name_callback);
            parent_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            parent_edit->when(FL_WHEN_ENTER_KEY);
          } // Fl_Input* parent_edit
          o->end();
        } // Fl_Scroll* o
        o->end();
      } // Fl_Tabs* o
      { special_tab = new Fl_Tabs(696, 78, 258, 621, "Special");
        special_tab->box(FL_NO_BOX);
        special_tab->hide();
        special_tab->resizable(NULL);
        special_tab->end();
      } // Fl_Tabs* special_tab
      { Fl_Tabs* o = new Fl_Tabs(693, 75, 222, 549, "Behaviors");
        o->hide();
        { behaviors_scroll = new Fl_Scroll(693, 75, 222, 549);
          behaviors_scroll->type(2);
          behaviors_scroll->end();
        } // Fl_Scroll* behaviors_scroll
        o->end();
      } // Fl_Tabs* o
      o->end();
    } // Fl_Tabs* o
    o->size_range(0, 0, 2048, 2048);
    o->end();
  } // Fl_Double_Window* o
  return w;
}

Fl_Double_Window* EditorWindow::create_light_window() {
  { light_window_object = new Fl_Double_Window(258, 716);
    light_window_object->labelsize(11);
    light_window_object->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(5, 30, 218, 159, "Settings:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { light_radius_edit = new Fl_Value_Input(18, 51, 165, 21, "Radius:");
        light_radius_edit->maximum(1e+07);
        light_radius_edit->step(2);
        light_radius_edit->callback((Fl_Callback*)edit_light_properties);
        light_radius_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_radius_edit
      { light_color_r = new Fl_Value_Input(18, 93, 45, 21, "Red:");
        light_color_r->step(0.1);
        light_color_r->callback((Fl_Callback*)edit_light_properties);
        light_color_r->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_color_r
      { light_color_g = new Fl_Value_Input(69, 93, 45, 21, "Green:");
        light_color_g->step(0.1);
        light_color_g->callback((Fl_Callback*)edit_light_properties);
        light_color_g->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_color_g
      { light_color_b = new Fl_Value_Input(123, 93, 45, 21, "Blue:");
        light_color_b->step(0.1);
        light_color_b->callback((Fl_Callback*)edit_light_properties);
        light_color_b->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_color_b
      { light_intensity_edit = new Fl_Value_Input(18, 162, 165, 21, "Intensity:");
        light_intensity_edit->step(0.05);
        light_intensity_edit->callback((Fl_Callback*)edit_light_properties);
        light_intensity_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        light_intensity_edit->soft(true);
      } // Fl_Value_Input* light_intensity_edit
      { Fl_Button* o = new Fl_Button(18, 117, 96, 24, "Choose color");
        o->labeltype(FL_ENGRAVED_LABEL);
        o->labelsize(11);
        o->callback((Fl_Callback*)choose_light_color);
      } // Fl_Button* o
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(5, 210, 218, 126, "Spotlight:");
      o->box(FL_ENGRAVED_BOX);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { light_spot_button = new Fl_Check_Button(15, 222, 159, 24, "Spot");
        light_spot_button->down_box(FL_DOWN_BOX);
        light_spot_button->callback((Fl_Callback*)edit_light_properties_chk_btn);
      } // Fl_Check_Button* light_spot_button
      { spot_angle_edit = new Fl_Value_Input(18, 264, 165, 21, "Spot Angle:");
        spot_angle_edit->maximum(90);
        spot_angle_edit->step(1);
        spot_angle_edit->callback((Fl_Callback*)edit_light_properties);
        spot_angle_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* spot_angle_edit
      { light_spot_exponent = new Fl_Value_Input(18, 306, 165, 21, "Spot Exponent:");
        light_spot_exponent->maximum(10);
        light_spot_exponent->step(0.01);
        light_spot_exponent->callback((Fl_Callback*)edit_light_properties);
        light_spot_exponent->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_spot_exponent
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(5, 357, 218, 174, "Shadow:");
      o->box(FL_ENGRAVED_BOX);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { light_shadows_button = new Fl_Check_Button(21, 369, 132, 27, "Shadows");
        light_shadows_button->down_box(FL_DOWN_BOX);
        light_shadows_button->callback((Fl_Callback*)edit_light_properties_chk_btn);
      } // Fl_Check_Button* light_shadows_button
      { light_shadow_bias_edit = new Fl_Value_Input(21, 414, 165, 21, "Shadow Bias:");
        light_shadow_bias_edit->maximum(10);
        light_shadow_bias_edit->step(0.1);
        light_shadow_bias_edit->callback((Fl_Callback*)edit_light_properties);
        light_shadow_bias_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_shadow_bias_edit
      { light_shadow_blur_edit = new Fl_Value_Input(21, 456, 165, 21, "Shadow Blur:");
        light_shadow_blur_edit->step(0.01);
        light_shadow_blur_edit->callback((Fl_Callback*)edit_light_properties);
        light_shadow_blur_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_shadow_blur_edit
      { light_shadow_quality_edit = new Fl_Value_Input(21, 498, 165, 21, "Shadow Quality:");
        light_shadow_quality_edit->maximum(4096);
        light_shadow_quality_edit->step(5);
        light_shadow_quality_edit->callback((Fl_Callback*)edit_light_properties);
        light_shadow_quality_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* light_shadow_quality_edit
      o->end();
    } // Fl_Group* o
    light_window_object->position(0,0);
    light_window_object->resizable(NULL);
    light_window_object->size_range(258, 726, 258, 726);
    light_window_object->end();
  } // Fl_Double_Window* light_window_object
  return light_window_object;
}

Fl_Double_Window* EditorWindow::create_publish_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(393, 117);
    w = o;
    o->user_data((void*)(this));
    { Fl_Clock* o = new Fl_Clock(252, -3, 126, 126);
      o->box(FL_NO_BOX);
    } // Fl_Clock* o
    { new Fl_Box(15, 12, 225, 27, "Publishing project. Please wait...");
    } // Fl_Box* o
    o->set_non_modal();
    o->end();
  } // Fl_Double_Window* o
  return w;
}

Fl_Double_Window* EditorWindow::create_object_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(360, 714);
    w = o;
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(5, 27, 218, 78, "Settings:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { object_mesh_edit = new Fl_Output(18, 51, 198, 21, "Mesh:");
        object_mesh_edit->color((Fl_Color)31);
        object_mesh_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Output* object_mesh_edit
      { object_invisible_button = new Fl_Check_Button(15, 78, 111, 24, "Invisible");
        object_invisible_button->down_box(FL_DOWN_BOX);
        object_invisible_button->callback((Fl_Callback*)edit_object_chk_btn);
      } // Fl_Check_Button* object_invisible_button
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(5, 132, 219, 249, "Physics:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { object_shape_choice = new Fl_Choice(18, 189, 195, 27, "Shape:");
        object_shape_choice->down_box(FL_BORDER_BOX);
        object_shape_choice->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        object_shape_choice->menu(menu_object_shape_choice);
      } // Fl_Choice* object_shape_choice
      { object_ghost_button = new Fl_Check_Button(18, 222, 78, 21, "Ghost");
        object_ghost_button->down_box(FL_DOWN_BOX);
        object_ghost_button->callback((Fl_Callback*)edit_object_chk_btn);
      } // Fl_Check_Button* object_ghost_button
      { object_mass_edit = new Fl_Value_Input(18, 261, 195, 21, "Mass:");
        object_mass_edit->callback((Fl_Callback*)edit_object_properties);
        object_mass_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* object_mass_edit
      { object_fric_edit = new Fl_Value_Input(18, 306, 195, 21, "Friction:");
        object_fric_edit->callback((Fl_Callback*)edit_object_properties);
        object_fric_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* object_fric_edit
      { object_rest_edit = new Fl_Value_Input(18, 351, 195, 21, "Restitution:");
        object_rest_edit->callback((Fl_Callback*)edit_object_properties);
        object_rest_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* object_rest_edit
      { object_physics_button = new Fl_Check_Button(18, 147, 204, 21, "Physics");
        object_physics_button->down_box(FL_DOWN_BOX);
        object_physics_button->callback((Fl_Callback*)edit_object_chk_btn);
      } // Fl_Check_Button* object_physics_button
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(6, 403, 219, 32, "Pivot:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { xpivot_edit = new Fl_Value_Input(27, 408, 51, 21, "X:");
        xpivot_edit->maximum(1000);
        xpivot_edit->step(1);
        xpivot_edit->callback((Fl_Callback*)edit_object_properties);
      } // Fl_Value_Input* xpivot_edit
      { ypivot_edit = new Fl_Value_Input(96, 408, 51, 21, "Y:");
        ypivot_edit->maximum(1000);
        ypivot_edit->step(1);
        ypivot_edit->callback((Fl_Callback*)edit_object_properties);
      } // Fl_Value_Input* ypivot_edit
      { zpivot_edit = new Fl_Value_Input(165, 407, 51, 21, "Z:");
        zpivot_edit->maximum(1000);
        zpivot_edit->step(1);
        zpivot_edit->callback((Fl_Callback*)edit_object_properties);
      } // Fl_Value_Input* zpivot_edit
      o->end();
    } // Fl_Group* o
    o->end();
  } // Fl_Double_Window* o
  return w;
}

Fl_Double_Window* EditorWindow::create_camera_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(258, 660);
    w = o;
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(5, 27, 218, 42, "Clear Color:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { camera_color_r_edit = new Fl_Value_Input(33, 39, 36, 18, "R:");
        camera_color_r_edit->step(0.01);
        camera_color_r_edit->callback((Fl_Callback*)edit_camera_properties);
      } // Fl_Value_Input* camera_color_r_edit
      { camera_color_g_edit = new Fl_Value_Input(93, 39, 36, 18, "G:");
        camera_color_g_edit->step(0.01);
        camera_color_g_edit->callback((Fl_Callback*)edit_camera_properties);
      } // Fl_Value_Input* camera_color_g_edit
      { camera_color_b_edit = new Fl_Value_Input(156, 39, 36, 18, "B:");
        camera_color_b_edit->step(0.01);
        camera_color_b_edit->callback((Fl_Callback*)edit_camera_properties);
      } // Fl_Value_Input* camera_color_b_edit
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(5, 96, 218, 171, "Settings:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { camera_ortho_button = new Fl_Check_Button(15, 237, 84, 24, "Ortho");
        camera_ortho_button->down_box(FL_DOWN_BOX);
        camera_ortho_button->callback((Fl_Callback*)edit_camera_properties_chk_btn);
      } // Fl_Check_Button* camera_ortho_button
      { camera_fov_edit = new Fl_Value_Input(15, 120, 117, 21, "FOV:");
        camera_fov_edit->callback((Fl_Callback*)edit_camera_properties);
        camera_fov_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* camera_fov_edit
      { camera_clipping_near_edit = new Fl_Value_Input(15, 165, 117, 21, "Clipping Near:");
        camera_clipping_near_edit->callback((Fl_Callback*)edit_camera_properties);
        camera_clipping_near_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* camera_clipping_near_edit
      { camera_clipping_far_edit = new Fl_Value_Input(15, 210, 117, 21, "Clipping Far:");
        camera_clipping_far_edit->callback((Fl_Callback*)edit_camera_properties);
        camera_clipping_far_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* camera_clipping_far_edit
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(5, 321, 218, 81, "Fog:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { camera_fog_button = new Fl_Check_Button(12, 327, 87, 21, "Fog");
        camera_fog_button->down_box(FL_DOWN_BOX);
        camera_fog_button->callback((Fl_Callback*)edit_camera_properties_chk_btn);
      } // Fl_Check_Button* camera_fog_button
      { camera_fog_distance_edit = new Fl_Value_Input(15, 372, 117, 21, "Fog Distance:");
        camera_fog_distance_edit->callback((Fl_Callback*)edit_camera_properties);
        camera_fog_distance_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* camera_fog_distance_edit
      o->end();
    } // Fl_Group* o
    o->end();
  } // Fl_Double_Window* o
  return w;
}

Fl_Double_Window* EditorWindow::create_text_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(261, 720);
    w = o;
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(6, 21, 210, 285, "Settings:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { text_text_edit = new Fl_Text_Editor(12, 42, 195, 126, "Text:");
        text_text_edit->box(FL_DOWN_BOX);
        text_text_edit->callback((Fl_Callback*)edit_text_properties);
        text_text_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        text_text_edit->when(FL_WHEN_CHANGED);
      } // Fl_Text_Editor* text_text_edit
      { text_font_edit = new Fl_Input(12, 195, 195, 21, "Font:");
        text_font_edit->callback((Fl_Callback*)edit_text_properties);
        text_font_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        text_font_edit->when(FL_WHEN_ENTER_KEY);
      } // Fl_Input* text_font_edit
      { Fl_Button* o = new Fl_Button(12, 219, 195, 24, "Find Font");
        o->callback((Fl_Callback*)text_find_font_callback);
      } // Fl_Button* o
      { text_size_edit = new Fl_Value_Input(15, 273, 51, 24, "Size:");
        text_size_edit->maximum(100);
        text_size_edit->step(1);
        text_size_edit->value(10);
        text_size_edit->callback((Fl_Callback*)edit_text_properties);
        text_size_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* text_size_edit
      { text_alignment_chooser = new Fl_Choice(84, 270, 123, 27, "Alignment:");
        text_alignment_chooser->down_box(FL_BORDER_BOX);
        text_alignment_chooser->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        text_alignment_chooser->menu(menu_text_alignment_chooser);
      } // Fl_Choice* text_alignment_chooser
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(6, 336, 210, 78, "Color:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { text_r = new Fl_Value_Input(27, 345, 48, 21, "R:");
        text_r->step(0.01);
        text_r->callback((Fl_Callback*)edit_text_properties);
      } // Fl_Value_Input* text_r
      { text_g = new Fl_Value_Input(96, 345, 48, 21, "G:");
        text_g->step(0.01);
        text_g->callback((Fl_Callback*)edit_text_properties);
      } // Fl_Value_Input* text_g
      { text_b = new Fl_Value_Input(27, 372, 48, 21, "A:");
        text_b->step(0.01);
        text_b->callback((Fl_Callback*)edit_text_properties);
      } // Fl_Value_Input* text_b
      { text_a = new Fl_Value_Input(162, 345, 48, 21, "B:");
        text_a->step(0.01);
        text_a->callback((Fl_Callback*)edit_text_properties);
      } // Fl_Value_Input* text_a
      { Fl_Button* o = new Fl_Button(96, 372, 114, 24, "Choose color");
        o->labeltype(FL_ENGRAVED_LABEL);
        o->labelsize(11);
        o->callback((Fl_Callback*)choose_text_color);
      } // Fl_Button* o
      o->end();
    } // Fl_Group* o
    o->end();
  } // Fl_Double_Window* o
  return w;
}

Fl_Double_Window* EditorWindow::create_sound_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(258, 720);
    w = o;
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(5, 24, 218, 279, "Settings:");
      o->box(FL_ENGRAVED_FRAME);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { sound_file_edit = new Fl_Output(12, 51, 204, 21, "Sound file:");
        sound_file_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Output* sound_file_edit
      { sound_loop_button = new Fl_Check_Button(12, 78, 204, 21, "Loop");
        sound_loop_button->down_box(FL_DOWN_BOX);
        sound_loop_button->callback((Fl_Callback*)edit_sound_chk_btn);
      } // Fl_Check_Button* sound_loop_button
      { sound_pitch_edit = new Fl_Value_Input(15, 120, 87, 21, "Pitch:");
        sound_pitch_edit->value(1);
        sound_pitch_edit->callback((Fl_Callback*)edit_sound_callback);
        sound_pitch_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* sound_pitch_edit
      { sound_gain_edit = new Fl_Value_Input(15, 162, 87, 21, "Gain:");
        sound_gain_edit->value(1);
        sound_gain_edit->callback((Fl_Callback*)edit_sound_callback);
        sound_gain_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* sound_gain_edit
      { sound_radius_edit = new Fl_Value_Input(15, 204, 87, 21, "Radius:");
        sound_radius_edit->value(10);
        sound_radius_edit->callback((Fl_Callback*)edit_sound_callback);
        sound_radius_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* sound_radius_edit
      { sound_rolloff_edit = new Fl_Value_Input(15, 249, 87, 21, "Rolloff:");
        sound_rolloff_edit->value(1);
        sound_rolloff_edit->callback((Fl_Callback*)edit_sound_callback);
        sound_rolloff_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Input* sound_rolloff_edit
      { sound_relative_button = new Fl_Check_Button(15, 276, 204, 21, "Relative");
        sound_relative_button->down_box(FL_DOWN_BOX);
        sound_relative_button->callback((Fl_Callback*)edit_sound_chk_btn);
      } // Fl_Check_Button* sound_relative_button
      o->end();
    } // Fl_Group* o
    o->end();
  } // Fl_Double_Window* o
  return w;
}

Fl_Double_Window* EditorWindow::create_scene_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(273, 87, "New Scene");
    w = o;
    o->user_data((void*)(this));
    { Fl_Button* o = new Fl_Button(174, 48, 87, 27, "Ok");
      o->callback((Fl_Callback*)new_scene_ok_callback);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(9, 45, 78, 27, "Cancel");
      o->callback((Fl_Callback*)new_scene_cancel_callback);
    } // Fl_Button* o
    { scene_name_input = new Fl_Input(105, 12, 156, 27, "Scene Name:");
    } // Fl_Input* scene_name_input
    o->set_modal();
    o->end();
  } // Fl_Double_Window* o
  return w;
}
extern EditorWindow window;

void new_scene_ok_callback(Fl_Button* button,void*) {
  ::window.scene_name = ::window.scene_name_input->value();
  
  button->parent()->hide();
}

void new_scene_cancel_callback(Fl_Button* button,void*) {
  Fl_Window* window = (Fl_Window*) button->parent();
  
  ::window.scene_name = "";
  window->hide();
}

Fl_Double_Window* SceneSetupDlg::create_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(405, 132, "Scene setup");
    w = o;
    o->user_data((void*)(this));
    { scene_name_edit = new Fl_Input(3, 21, 150, 21, "Scene Name:");
      scene_name_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Input* scene_name_edit
    { lua_script_edit = new Fl_Input(6, 66, 363, 21, "LUA-Script:");
      lua_script_edit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Input* lua_script_edit
    { Fl_Button* o = new Fl_Button(372, 66, 24, 21, "...");
      o->callback((Fl_Callback*)find_file_callback);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(291, 96, 108, 27, "Ok");
      o->callback((Fl_Callback*)ok_button_callback);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(9, 96, 108, 27, "Cancel");
      o->callback((Fl_Callback*)cancel_button_callback);
    } // Fl_Button* o
    o->set_modal();
    o->end();
  } // Fl_Double_Window* o
  return w;
}

void ok_button_callback(Fl_Button* button, void*) {
  button->parent()->label("Success");
  button->parent()->hide();
}

void cancel_button_callback(Fl_Button* button, void*) {
  button->parent()->hide();
}

void find_file_callback(Fl_Button* button, void*) {
  const char* filename = fl_native_file_chooser("Choose file", "*.lua", NULL, Fl_Native_File_Chooser::BROWSE_FILE);
  
  int children = button->parent()->children();
  Fl_Input* input;
  
  for(int i = 0; i < children; i++)
  {
  	if(!strcmp(button->parent()->child(i)->label(), "LUA-Script:"))
  	{
  		input = (Fl_Input*) button->parent()->child(i);
  		input->value(filename);
  		return;	
  	}
  }
}

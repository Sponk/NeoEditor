#include "Callbacks.h"
#include <cstdio>

#ifndef WIN32
#include <unistd.h>
#endif

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Tree_Item.H>
#include <FL/Fl_Tree_Prefs.H>

#include <MEngine.h>
#include <MLoaders/MImageLoader.h>
#include <MLog.h>
#include <MThread.h>
#include "../MFilesUpdate/MFilesUpdate.h"
#include "../Maratis/Maratis.h"
#include "../MLoaders/MAssimpMeshLoader.h"
#include <MCore.h>
#include <MLog.h>
#include "../MPublish/MPublisher.h"
#include "MainWindow.h"

open_project_t current_project;
bool reload_editor = false;

extern Fl_Double_Window* main_window;
extern EditorWindow window;

Fl_Window* object_window;
Fl_Window* light_window;
Fl_Window* text_window;
Fl_Window* sound_window;
Fl_Window* camera_window;

Fl_Text_Buffer textbuf;

char labels[][2] = {{"X"},{"Y"},{"Z"},{"W"}};

extern MSemaphore updateSemaphore;
extern const char* executable;

bool update_name = true;

const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type)
{
    Fl_Native_File_Chooser* dlg = new Fl_Native_File_Chooser();
    dlg->title(title);
    dlg->filter(files);
    dlg->type(type);
    dlg->directory(dir);

    if(dlg->show() == 0)
        return dlg->filename();

    return NULL;
}

void quit_callback(Fl_Menu_*, void*)
{
    if(fl_ask("Do you really want to exit?"))
        exit(0);
}

void add_to_tree(MObject3d* entity, std::string path)
{
    MScene* scene = MEngine::getInstance()->getLevel()->getCurrentScene();

    if(entity)
    {
        path += "/" + std::string(entity->getName());

        if(!window.scene_tree->find_item(path.c_str()))
            window.scene_tree->add(path.c_str());

        for(int i = 0; i < entity->getChildsNumber(); i++)
        {
            if(!window.scene_tree->find_item((path + "/" + std::string(entity->getChild(i)->getName())).c_str()))
                window.scene_tree->add((path + "/" + std::string(entity->getChild(i)->getName())).c_str());

            add_to_tree(scene->getEntityByName(entity->getChild(i)->getName()), path.c_str());
        }
    }
}

void change_scene_callback(Fl_Menu_*, long index)
{
    // MLOG_INFO("Changing to scene " << index);

    MLevel* level = MEngine::getInstance()->getLevel();
    Maratis::getInstance()->autoSave();
    level->setCurrentSceneId(index);
    Maratis::getInstance()->clearSelectedObjects();

    window.glbox->redraw();
}

void update_scene_tree()
{
    window.scene_tree->clear();

    MLevel* level = MEngine::getInstance()->getLevel();
    MScene* scene = level->getCurrentScene();

    unsigned int index = 0;

    window.scene_tree->begin();
    for(int i = 0; i < scene->getObjectsNumber(); i++)
    {
        MObject3d* entity = scene->getObjectByIndex(i);
        if(entity && entity->getParent() == NULL)
        {
            add_to_tree(entity, "");
        }
    }

    window.scene_tree->end();
    window.scene_tree->sortorder();

    window.scenes_menu->clear();
    for(int i = 0; i < level->getScenesNumber(); i++)
    {
        scene = level->getSceneByIndex(i);
        level->getSceneIndexByName(scene->getName(), &index);

        window.scenes_menu->add(scene->getName(), 0, (Fl_Callback*) change_scene_callback, (void*) index);
    }

    window.scenes_menu->value(level->getCurrentSceneId());

    update_behavior_menu();
	window.scene_tree->redraw();
}

void open_level_callback(Fl_Menu_*, void*)
{
    const char* filename = fl_native_file_chooser("Open level", "*.level", current_project.path.c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(filename)
    {
        Maratis::getInstance()->loadLevel(filename);

        update_scene_tree();
        current_project.level = filename;
    }

    reload_editor = true;

    window.glbox->redraw();
    window.scene_tree->redraw();
}

void open_project_callback(Fl_Menu_*, void*)
{
    if(!current_project.path.empty() && !fl_ask("You need to close the current project. Do you want to proceed?"))
        return;

    const char* home = NULL;

#ifndef WIN32
    home = getenv("HOME");
#else
    home = getenv("USERPROFILE");
#endif

    const char* filename = fl_native_file_chooser("Open project", "*.mproj", home, Fl_Native_File_Chooser::BROWSE_FILE);

    if(filename)
    {
        current_project.changed = false;
        current_project.path = filename;
        current_project.file_path = filename;

#ifndef WIN32
        current_project.path = current_project.path.erase(current_project.path.find_last_of("/")+1, current_project.path.length());
#else
        current_project.path = current_project.path.erase(current_project.path.find_last_of("\\")+1, current_project.path.length());
#endif
        Maratis::getInstance()->loadProject(filename);

        current_project.level = Maratis::getInstance()->getCurrentLevel();
        MLOG_INFO("Loaded level " << current_project.level);

        update_scene_tree();
    }

    reload_editor = true;

    window.glbox->redraw();
    window.scene_tree->redraw();
}

float rotation_speed = 1.0;

void rotation_speed_callback(Fl_Value_Input* input, void*)
{
    rotation_speed = input->value() / 10;
}

float translation_speed = 1.0;

void translation_speed_callback(Fl_Value_Input* input, void*)
{
    translation_speed = input->value()/10;
}

void set_edit_type(Fl_Round_Button* button, long c)
{
    switch(c)
    {
    case 't':
            Maratis::getInstance()->setTransformMode(M_TRANSFORM_POSITION);
        break;

    case 'r':
            Maratis::getInstance()->setTransformMode(M_TRANSFORM_ROTATION);
        break;

    case 's':
            Maratis::getInstance()->setTransformMode(M_TRANSFORM_SCALE);
        break;
    }

    window.glbox->redraw();
}

void remove_window(Fl_Window* win)
{
    if(win)
    {
        win->hide();
        window.special_tab->remove(win);
        Fl::delete_widget(win);
    }
}

void* get_variable_pointer(const char* bname, const char* vname, MObject3d* object)
{
    if(!vname || !bname || !object)
        return NULL;

    MBehavior* behavior;
    for(int i = 0; i < object->getBehaviorsNumber(); i++)
    {
        behavior = object->getBehavior(i);
        if(!strcmp(behavior->getName(), bname))
        {
            for(int j = 0; j < behavior->getVariablesNumber(); j++)
            {
                if(!strcmp(behavior->getVariable(j).getName(), vname))
                {
                    return behavior->getVariable(j).getPointer();
                }
            }
        }
    }
}

void behavior_float_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    *((float*) get_variable_pointer(data, input->label(), object)) = input->value();
}

void behavior_int_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    *(int*) get_variable_pointer(data, input->label(), object) = input->value();
}

void behavior_bool_callback(Fl_Check_Button* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    *(bool*) get_variable_pointer(data, input->label(), object) = input->value();
}

void behavior_string_callback(Fl_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    ((MString*) get_variable_pointer(data, input->label(), object))->set(input->value());
}

void behavior_vector4_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    Fl_Group* group = input->parent()->parent();
    if(!group)
    {
        MLOG_WARNING("Could not update behavior! The input does not belong to any group!");
        return;
    }

    MVector4* vector = (MVector4*) get_variable_pointer(group->label(), data, object);

    if(!vector)
    {
        MLOG_WARNING("Could not update behavior! vector == NULL!");
        return;
    }

    switch(input->label()[0])
    {
    case 'X':
        vector->x = input->value();
        break;
    case 'Y':
        vector->y = input->value();
        break;
    case 'Z':
        vector->z = input->value();
        break;
    case 'W':
        vector->w = input->value();
        break;
    }
}

void behavior_vector3_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    Fl_Group* group = input->parent()->parent();
    if(!group)
    {
        MLOG_WARNING("Could not update behavior! The input does not belong to any group!");
        return;
    }

    MVector3* vector = (MVector3*) get_variable_pointer(group->label(), data, object);

    if(!vector)
    {
        MLOG_WARNING("Could not update behavior! vector == NULL!");
        return;
    }

    switch(input->label()[0])
    {
    case 'X':
        vector->x = input->value();
        break;
    case 'Y':
        vector->y = input->value();
        break;
    case 'Z':
        vector->z = input->value();
        break;
    }
}

void behavior_vector2_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    Fl_Group* group = input->parent()->parent();
    if(!group)
    {
        MLOG_WARNING("Could not update behavior! The input does not belong to any group!");
        return;
    }

    MVector2* vector = (MVector2*) get_variable_pointer(group->label(), data, object);

    if(!vector)
    {
        MLOG_WARNING("Could not update behavior! vector == NULL!");
        return;
    }

    switch(input->label()[0])
    {
    case 'X':
        vector->x = input->value();
        break;
    case 'Y':
        vector->y = input->value();
        break;
    }
}

void create_behavior_ui(MObject3d* object);

void remove_behavior(Fl_Button*, long behavior)
{
    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object)
        return;

    Maratis::getInstance()->autoSave();
    object->deleteBehavior(behavior);

    update_scene_tree();
    create_behavior_ui(object);
    window.behaviors_scroll->redraw();
}

void create_behavior_ui(MObject3d* object)
{
    if(object == NULL)
        return;

    // Create behavior UI
    int num_behaviors = object->getBehaviorsNumber();
    window.behaviors_scroll->clear();

    int height = 0;

    for(int b = 0; b < num_behaviors; b++)
    {
        window.behaviors_scroll->begin();

        MBehavior* behavior = object->getBehavior(b);
        int num_variables = behavior->getVariablesNumber();

        Fl_Group* group = new Fl_Group(window.behaviors_scroll->x()+4,window.behaviors_scroll->y()+height+20, 218, 0);
        window.behaviors_scroll->add(group);

        int x = group->x();
        int y = group->y();

        group->label(behavior->getName());
        group->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        group->box(FL_ENGRAVED_FRAME);
        group->resizable(NULL);

        int old_height = height;
        height = 0;

        for(int i = 0; i < num_variables; i++)
        {
            MVariable var = behavior->getVariable(i);

            switch(var.getType())
            {
            case M_VARIABLE_FLOAT:
                {
                    Fl_Value_Input* input = new Fl_Value_Input(x+5,y+height+20, 90, 22, var.getName());
                    input->align(Fl_Align(FL_ALIGN_TOP_LEFT));
                    input->value(*(float*)var.getPointer());

                    input->callback((Fl_Callback*) behavior_float_callback, (void*) behavior->getName());

                    group->add(input);

                    height += 47;
                }
                break;

            case M_VARIABLE_UINT:
            case M_VARIABLE_INT:
                {
                    Fl_Value_Input* input = new Fl_Value_Input(x+5,y+height+20, 90, 22, var.getName());
                    input->align(Fl_Align(FL_ALIGN_TOP_LEFT));
                    input->value(*(float*)var.getPointer());
                    input->step(1);

                    input->callback((Fl_Callback*) behavior_int_callback, (void*) behavior->getName());

                    group->add(input);

                    height += 47;
                }
                break;

            case M_VARIABLE_STRING:
                {
                    Fl_Input* input = new Fl_Input(x+5,y+height+20, 205, 22, var.getName());
                    input->align(Fl_Align(FL_ALIGN_TOP_LEFT));
                    input->value(((MString*)var.getPointer())->getSafeString());

                    input->callback((Fl_Callback*) behavior_string_callback, (void*) behavior->getName());

                    group->add(input);

                    height += 47;
                }
                break;

            case M_VARIABLE_VEC3:
                {
                    Fl_Group* vec_group = new Fl_Group(x+5, y+height+20, 205, 30, var.getName());
                    vec_group->align(Fl_Align(FL_ALIGN_TOP_LEFT));

                    height += 3;

                    MVector3* vector = (MVector3*) var.getPointer();

                    float values[3] = {vector->x, vector->y, vector->z};

                    for(int i = 0; i < 3; i++)
                    {
                        Fl_Value_Input* input = new Fl_Value_Input(x+65*i+7,y+height+30, 65, 22, &labels[i][0]);
                        input->align(Fl_Align(FL_ALIGN_TOP_LEFT));
                        input->value(values[i]);

                        input->callback((Fl_Callback*) behavior_vector3_callback, (void*) var.getName());

                        vec_group->add(input);
                    }

                    vec_group->end();
                    group->add(vec_group);
                    height += 57;
                }
                break;

            case M_VARIABLE_VEC2:
                {
                    Fl_Group* vec_group = new Fl_Group(x+5, y+height+20, 205, 30, var.getName());
                    vec_group->align(Fl_Align(FL_ALIGN_TOP_LEFT));

                    height += 3;

                    MVector2* vector = (MVector2*) var.getPointer();

                    float values[2] = {vector->x, vector->y};

                    for(int i = 0; i < 2; i++)
                    {
                        Fl_Value_Input* input = new Fl_Value_Input(x+65*i+7,y+height+30, 65, 22, &labels[i][0]);
                        input->align(Fl_Align(FL_ALIGN_TOP_LEFT));
                        input->value(values[i]);

                        input->callback((Fl_Callback*) behavior_vector2_callback, (void*) var.getName());

                        vec_group->add(input);
                    }

                    vec_group->end();
                    group->add(vec_group);
                    height += 57;
                }
                break;

            case M_VARIABLE_VEC4:
                {
                    Fl_Group* vec_group = new Fl_Group(x+5, y+height+20, 205, 30, var.getName());
                    vec_group->align(Fl_Align(FL_ALIGN_TOP_LEFT));

                    height += 3;

                    MVector4* vector = (MVector4*) var.getPointer();

                    float values[4] = {vector->x, vector->y, vector->z, vector->w};

                    for(int i = 0; i < 4; i++)
                    {
                        Fl_Value_Input* input = new Fl_Value_Input(x+50*i+7,y+height+30, 50, 22, &labels[i][0]);
                        input->align(Fl_Align(FL_ALIGN_TOP_LEFT));
                        input->value(values[i]);

                        input->callback((Fl_Callback*) behavior_vector4_callback, (void*) var.getName());

                        vec_group->add(input);
                    }

                    vec_group->end();
                    group->add(vec_group);
                    height += 57;
                }
                break;

            case M_VARIABLE_BOOL:
                {
                    Fl_Check_Button* button = new Fl_Check_Button(x+5,y+height+20, 205, 22, var.getName());
                    button->value(*(bool*)var.getPointer());

                    button->callback((Fl_Callback*) behavior_bool_callback, (void*) behavior->getName());

                    group->add(button);

                    height += 47;
                }

                break;

            default:
                MLOG_WARNING("Behavior variable type not yet supported!");
            }
        }

        Fl_Button* remove_button = new Fl_Button(x+5, y+height, 205, 22, "Remove");
        remove_button->callback((Fl_Callback*) remove_behavior, (void*) (object->getBehaviorsNumber()-1));
        group->add(remove_button);

        height += 27;

        group->size(218, height);
        height += old_height+20;

        group->end();
        window.behaviors_scroll->end();
    }

    window.behaviors_scroll->redraw();
}

#define DELETE_WINDOW(x) remove_window(x); x = NULL

void scene_tree_callback(Fl_Tree* tree, void*)
{
    Fl_Tree_Item* item = tree->first_selected_item();
    if(!item)
        return;

    const char* name = item->label();

    if(!name || !strcmp(name, "ROOT"))
        return;

    MObject3d* object = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(name);

    if(!object)
    {
        MLOG_ERROR("Object does not exist!");
        return;
    }

    // Translate to object for easier navigation
    // TODO: Check for real double click!
    /*if(Fl::event_clicks())
    {
        MOCamera* vue = Maratis::getInstance()->getPerspectiveVue();
        vue->setPosition(object->getPosition() - MVector3(10,0,0));
        vue->updateMatrix();
        window.glbox->redraw();
    }*/

    // TODO: Multiple selection!
    //Maratis::getInstance()->clearSelectedObjects();
    //Maratis::getInstance()->addSelectedObject(object);

    MVector3 position = object->getPosition();
    MVector3 rotation = object->getEulerRotation();
    MVector3 scale = object->getScale();

    if(update_name)
        create_behavior_ui(object);

    if(object->getType() == M_OBJECT3D_ENTITY)
    {
        DELETE_WINDOW(text_window);
        DELETE_WINDOW(light_window);
        DELETE_WINDOW(sound_window);
        DELETE_WINDOW(camera_window);

        if(object_window == NULL)
        {
            object_window = window.create_object_window();

            object_window->position(window.special_tab->x()-3, window.special_tab->y());
            window.special_tab->add(object_window);
        }

        MOEntity* entity = (MOEntity*) object;
        window.object_mesh_edit->value(entity->getMeshRef()->getFilename());

        if(update_name)
        {
            window.object_invisible_button->value(!entity->isVisible());
        }

        MPhysicsProperties* phys = entity->getPhysicsProperties();

        if(phys)
        {
            window.object_mass_edit->value(phys->getMass());
            window.object_fric_edit->value(phys->getFriction());
            window.object_rest_edit->value(phys->getRestitution());

            MPhysicsConstraint* constraint = phys->getConstraint();

            if(constraint)
            {
                MVector3 pivot;
                pivot = constraint->pivot;
                window.xpivot_edit->value(pivot.x);
                window.ypivot_edit->value(pivot.y);
                window.zpivot_edit->value(pivot.z);
            }
            else
            {
                window.xpivot_edit->deactivate();
                window.ypivot_edit->deactivate();
                window.zpivot_edit->deactivate();
            }

            if(update_name)
            {
                window.object_ghost_button->value(phys->isGhost());
                window.object_physics_button->value(1);
                window.object_shape_choice->value(phys->getCollisionShape());
            }
        }
        else if(update_name)
        {
            window.object_physics_button->value(0);
        }
    }
    else if(object->getType() == M_OBJECT3D_LIGHT)
    {
        MOLight* light = (MOLight*) object;

        DELETE_WINDOW(text_window);
        DELETE_WINDOW(sound_window);
        DELETE_WINDOW(object_window);
        DELETE_WINDOW(camera_window);

        if(light_window == NULL)
        {
            light_window = window.create_light_window();

            light_window->position(window.special_tab->x()-3, window.special_tab->y());
            window.special_tab->add(light_window);
        }

        if(update_name)
        {
            // TODO: HACK!!
            if(light->getSpotAngle() >= 180)
                window.light_spot_button->value(0);
            else
                window.light_spot_button->value(1);

            window.light_shadows_button->value(light->isCastingShadow());
        }

        window.light_radius_edit->value(light->getRadius());

        MVector3 color = light->getColor();
        window.light_color_r->value(color.x);
        window.light_color_g->value(color.y);
        window.light_color_b->value(color.z);

        window.light_intensity_edit->value(light->getIntensity());
        window.spot_angle_edit->value(light->getSpotAngle());
        window.light_spot_exponent->value(light->getSpotExponent());

        window.light_shadow_bias_edit->value(light->getShadowBias());
        window.light_shadow_blur_edit->value(light->getShadowBlur());
        window.light_shadow_quality_edit->value(light->getShadowQuality());

        position = light->getPosition();
    }
    else if(object->getType() == M_OBJECT3D_CAMERA)
    {
        DELETE_WINDOW(text_window);
        DELETE_WINDOW(light_window);
        DELETE_WINDOW(sound_window);
        DELETE_WINDOW(object_window);

        if(camera_window == NULL)
        {
            camera_window = window.create_camera_window();

            camera_window->position(window.special_tab->x()-3, window.special_tab->y());
            window.special_tab->add(camera_window);
        }

        MOCamera* camera = (MOCamera*) object;
        MVector3 clearColor = camera->getClearColor();

        window.camera_color_r_edit->value(clearColor.x);
        window.camera_color_g_edit->value(clearColor.y);
        window.camera_color_b_edit->value(clearColor.z);

        window.camera_fov_edit->value(camera->getFov());
        window.camera_clipping_near_edit->value(camera->getClippingNear());
        window.camera_clipping_far_edit->value(camera->getClippingFar());

        window.camera_fog_distance_edit->value(camera->getFogDistance());

        if(update_name)
        {
            window.camera_ortho_button->value(camera->isOrtho());
            window.camera_fog_button->value(camera->hasFog());
        }
    }
    else if(object->getType() == M_OBJECT3D_TEXT)
    {
        DELETE_WINDOW(camera_window);
        DELETE_WINDOW(light_window);
        DELETE_WINDOW(sound_window);
        DELETE_WINDOW(object_window);

        if(text_window == NULL)
        {
            text_window = window.create_text_window();

            text_window->position(window.special_tab->x()-3, window.special_tab->y());
            window.special_tab->add(text_window);
        }

        MOText* text = (MOText*) object;

        window.text_text_edit->buffer(&textbuf);

        if(update_name)
        {
            window.text_text_edit->buffer()->remove(0, window.text_text_edit->buffer()->length());
            window.text_text_edit->buffer()->append(text->getText());

            window.text_font_edit->value(text->getFontRef()->getFilename());
        }

        window.text_size_edit->value(text->getSize());
        window.text_alignment_chooser->value(text->getAlign());

        MVector4 color = text->getColor();
        window.text_r->value(color.x);
        window.text_g->value(color.y);
        window.text_b->value(color.z);
        window.text_a->value(color.w);
    }
    else if(object->getType() == M_OBJECT3D_SOUND)
    {
        DELETE_WINDOW(text_window);
        DELETE_WINDOW(light_window);
        DELETE_WINDOW(camera_window);
        DELETE_WINDOW(object_window);

        if(sound_window == NULL)
        {
            sound_window = window.create_sound_window();

            sound_window->position(window.special_tab->x()-3, window.special_tab->y());
            window.special_tab->add(sound_window);
        }

        MOSound* sound = (MOSound*) object;

        window.sound_file_edit->value(sound->getSoundRef()->getFilename());
        window.sound_pitch_edit->value(sound->getPitch());
        window.sound_gain_edit->value(sound->getGain());
        window.sound_radius_edit->value(sound->getRadius());
        window.sound_rolloff_edit->value(sound->getRolloff());

        if(update_name)
        {
            window.sound_loop_button->value(sound->isLooping());
            window.sound_relative_button->value(sound->isRelative());
        }
    }

    if(update_name)
    {
        MObject3d* parent = object->getParent();

        if(parent)
            window.parent_edit->value(parent->getName());
        else
            window.parent_edit->value("none");

        window.name_edit->value(name);
    }

    window.xpos_edit->value(position.x);
    window.ypos_edit->value(position.y);
    window.zpos_edit->value(position.z);

    window.xrot_edit->value(rotation.x);
    window.yrot_edit->value(rotation.y);
    window.zrot_edit->value(rotation.z);

    window.xscale_edit->value(scale.x);
    window.yscale_edit->value(scale.y);
    window.zscale_edit->value(scale.z);

    window.special_tab->redraw();
    window.glbox->redraw();
}

void edit_object_callback(Fl_Value_Input* input, long c)
{
    const char* name = window.name_edit->value();

    if(!name || !strcmp(name, ""))
    {
        MLOG_WARNING("No object selected! Can not edit!");
        return;
    }

    switch(c)
    {
    case 't':
            {
                MScene* scene = MEngine::getInstance()->getLevel()->getCurrentScene();
                MObject3d* entity = scene->getObjectByName(name);
                MVector3 position(window.xpos_edit->value(), window.ypos_edit->value(), window.zpos_edit->value());

                if(!entity)
                {
                    MLOG_ERROR("Object " << name << " does not exist!");
                    return;
                }


                entity->setPosition(position);
                entity->updateMatrix();
                }
        break;

    case 'r': {
                MScene* scene = MEngine::getInstance()->getLevel()->getCurrentScene();
                MObject3d* entity = scene->getObjectByName(name);

                MVector3 rotation(window.xrot_edit->value(), window.yrot_edit->value(), window.zrot_edit->value());

                if(!entity)
                {
                    MLOG_ERROR("Object " << name << " does not exist!");
                    return;
                }

                entity->setEulerRotation(rotation);
                entity->updateMatrix();
            }
        break;

    case 's':
            {
                    MScene* scene = MEngine::getInstance()->getLevel()->getCurrentScene();
                    MObject3d* entity = scene->getObjectByName(name);

                    MVector3 scale(window.xscale_edit->value(), window.yscale_edit->value(), window.zscale_edit->value());

                    if(!entity)
                    {
                        MLOG_ERROR("Object " << name << " does not exist!");
                        return;
                    }

                    entity->setScale(scale);
                    entity->updateMatrix();
                }
        break;

    case 'n':
            {

        }
        break;
    }

    window.glbox->redraw();
}

void edit_name_callback(Fl_Input*, void*)
{
    if(Maratis::getInstance()->getSelectedObjectsNumber() == 0)
        return;

    MObject3d* object = Maratis::getInstance()->getSelectedObjectByIndex(0);
    object->setName(window.name_edit->value());

    MObject3d* parent = object->getParent();

    if(parent == NULL && !strcmp(window.parent_edit->value(), "none"))
    {
        update_scene_tree();
        return;
    }
    else if(parent == NULL)
    {
        parent = MEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.parent_edit->value());

        if(!parent)
        {
            MLOG_ERROR("Can't set parent: Parent object not found!");
            fl_alert("Can't set parent: Parent object not found!");

            window.parent_edit->value("none");
            update_scene_tree();
            return;
        }

        Maratis::getInstance()->linkTwoObjects(parent, object);
    }
    else if(parent && !strcmp(window.parent_edit->value(), "none") || strlen(window.parent_edit->value()) == 0)
    {
        Maratis::getInstance()->unlinkTwoObjects(parent, object);
    }

    update_scene_tree();
}

void edit_light_properties(Fl_Value_Input *, void *)
{
    MOLight* light = MEngine::getInstance()->getLevel()->getCurrentScene()->getLightByName(window.name_edit->value());

    if(!light)
    {
        MLOG_WARNING("Light with the name '" << window.name_edit->value() << "' does not exist!");
        return;
    }

    if(!light_window)
    {
        MLOG_ERROR("There is no light selected! This should not happen!");
        return;
    }

    light->setRadius(window.light_radius_edit->value());
    light->setColor(MVector3(window.light_color_r->value(), window.light_color_g->value(), window.light_color_b->value()));

    light->setIntensity(window.light_intensity_edit->value());

    // TODO: HACK!!
    if(window.light_spot_button->value() == 1)
        light->setSpotAngle(window.spot_angle_edit->value());
    else
    {
        light->setSpotAngle(180);
    }

    light->setSpotExponent(window.light_spot_exponent->value());
    light->setShadowBias(window.light_shadow_bias_edit->value());
    light->setShadowBlur(window.light_shadow_blur_edit->value());
    light->setShadowQuality(window.light_shadow_quality_edit->value());

    window.glbox->redraw();
    window.special_tab->redraw();
}

void edit_light_properties_chk_btn(Fl_Check_Button * button, void *)
{    
    MOLight* light = MEngine::getInstance()->getLevel()->getCurrentScene()->getLightByName(window.name_edit->value());

    // TODO: HACK!!
    if(window.light_spot_button->value() == 1)
        light->setSpotAngle(window.spot_angle_edit->value());
    else
    {
        light->setSpotAngle(180);
        window.spot_angle_edit->value(180);
    }

    if(window.light_shadows_button->value() == 1)
    {
        light->castShadow(true);
        edit_light_properties(NULL, NULL);
    }
    else
    {
        light->castShadow(false);
    }

    window.glbox->redraw();
}

void choose_light_color(Fl_Button*, void*)
{
    double r = window.light_color_r->value();
    double g = window.light_color_g->value();
    double b = window.light_color_b->value();

    fl_color_chooser("Choose a color", r, g, b);

    window.light_color_r->value(r);
    window.light_color_g->value(g);
    window.light_color_b->value(b);

    edit_light_properties(NULL, NULL);
}

void publish_callback(Fl_Menu_*, void*)
{
    if(current_project.path.empty())
    {
        MLOG_ERROR("No project open!");
        fl_message("Can't publish: You need to create/open a project first!");
        return;
    }

    Maratis::getInstance()->publish();

    fl_message("Successfully published project!");
}

void save_level_callback(Fl_Menu_ *, long mode)
{
    MLOG_INFO("Saving level");
    const char* filename = NULL;
    switch(mode)
    {
    // Save
    case 0:
        if(current_project.level.empty())
        {
            filename = fl_native_file_chooser("Select a file to save", "*.level",  (current_project.path + "levels").c_str(),
                                              Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

            if(filename == NULL)
                return;

            current_project.level = filename;
        }

        Maratis::getInstance()->setCurrentLevel(current_project.level.c_str());

        break;
    // Save as
    case 1:
        filename = fl_native_file_chooser("Select a file to save", "*.level", (current_project.path + "levels").c_str(),
                                          Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

        if(filename == NULL)
            return;

        current_project.level = filename;
        Maratis::getInstance()->setCurrentLevel(current_project.level.c_str());

        break;
    }

    Maratis::getInstance()->save();
}

void new_project_callback(Fl_Menu_*, void*)
{
    const char* filename = fl_native_file_chooser("Select project file", "*.mproj", NULL, Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

    if(!filename)
        return;

    MLOG_INFO("Creating new project in " << filename);

    current_project.level = "";
    current_project.path = filename;

    Maratis::getInstance()->okNewProject(filename);
}

void new_level_callback(Fl_Menu_*, void*)
{
    current_project.level = "";

    Maratis::getInstance()->newLevel();
    //main_window->label("Untitled level");
}

void edit_shape_callback(Fl_Menu_* menu, long type)
{
    MOEntity * entity = MEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(window.name_edit->value());

    if(!entity || !entity->getPhysicsProperties())
        return;

    entity->getPhysicsProperties()->setCollisionShape((M_COLLISION_SHAPE_TYPE)type);
}

void edit_object_chk_btn(Fl_Check_Button*, void*)
{
    MOEntity * entity = MEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(window.name_edit->value());

    if(!entity)
        return;

    entity->setInvisible(window.object_invisible_button->value());

    MPhysicsProperties* phys = entity->getPhysicsProperties();

    if(phys && window.object_physics_button->value())
        phys->setGhost(window.object_ghost_button->value());
    else if(!phys && window.object_physics_button->value())
    {
        entity->createPhysicsProperties();
        phys = entity->getPhysicsProperties();
        phys->setGhost(window.object_ghost_button->value());
    }
    else if(phys && !window.object_physics_button->value())
    {
        entity->deletePhysicsProperties();
        return;
    }

    window.glbox->redraw();
}

void edit_object_properties(Fl_Value_Input*, void*)
{
    MOEntity * entity = MEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(window.name_edit->value());

    if(!entity)
        return;

    MPhysicsProperties* phys = entity->getPhysicsProperties();

    if(!phys)
        return;

    phys->setMass(window.object_mass_edit->value());
    phys->setFriction(window.object_fric_edit->value());
    phys->setRestitution(window.object_rest_edit->value());

    MPhysicsConstraint* constraint = phys->getConstraint();

    if(constraint)
    {
        constraint->pivot.x = window.xpivot_edit->value();
        constraint->pivot.y = window.ypivot_edit->value();
        constraint->pivot.z = window.zpivot_edit->value();
    }
}

void add_mesh_callback(Fl_Menu_*, void*)
{
    const char* filename = fl_native_file_chooser("Choose mesh", "*.mesh", (current_project.path + "meshs").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(filename)
    {
        Maratis::getInstance()->autoSave();
        Maratis::getInstance()->okAddEntity(filename);
        update_scene_tree();
    }

    window.glbox->redraw();
	update_scene_tree();
}

void add_light_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->autoSave();
    Maratis::getInstance()->addLight();
    update_scene_tree();

    window.glbox->redraw();
}

void edit_camera_properties(Fl_Value_Input*, void*)
{
    MOCamera* camera = MEngine::getInstance()->getLevel()->getCurrentScene()->getCameraByName(window.name_edit->value());

    if(!camera)
    {
        MLOG_ERROR("Camera object does not exist!");
        return;
    }

    camera->setClearColor(MVector3(window.camera_color_r_edit->value(), window.camera_color_g_edit->value(), window.camera_color_b_edit->value()));
    camera->setFov(window.camera_fov_edit->value());
    camera->setClippingNear(window.camera_clipping_near_edit->value());
    camera->setClippingFar(window.camera_clipping_far_edit->value());

    camera->setFogDistance(window.camera_fog_distance_edit->value());
}

void edit_camera_properties_chk_btn(Fl_Check_Button*, void*)
{
    MOCamera* camera = MEngine::getInstance()->getLevel()->getCurrentScene()->getCameraByName(window.name_edit->value());

    if(!camera)
    {
        MLOG_ERROR("Camera object does not exist!");
        return;
    }

    camera->enableOrtho(window.camera_ortho_button->value());
    camera->enableFog(window.camera_fog_button->value());
}

bool text_load_font(const char* name, MOText* text)
{
    if(!name || !text)
        return false;

    MLevel* level = MEngine::getInstance()->getLevel();
    MFontRef* ref = text->getFontRef();

    if(!strcmp(name, ref->getFilename()))
        return true;

    MFontRef* fontRef = level->loadFont(name);

    if(fontRef)
    {
        text->setFontRef(fontRef);
        return true;
    }
    else
    {
        MLOG_ERROR("Could not load font " << window.text_font_edit->value());
    }

    return false;
}

void text_find_font_callback(Fl_Button*, void*)
{
    const char* filename = fl_native_file_chooser("Choose font", "*.ttf", (current_project.path + "fonts").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);
    MLevel* level = MEngine::getInstance()->getLevel();
    MOText* text = level->getCurrentScene()->getTextByName(window.name_edit->value());

    if(!text)
    {
        MLOG_ERROR("Text field does not exist!");
        return;
    }

    if(!filename)
        return;

    if(!text_load_font(filename, text))
    {
        fl_alert("Could not load font!");
        return;
    }

    window.text_font_edit->value(filename);
    window.glbox->redraw();
}

void edit_text_properties(Fl_Widget*, void*)
{
    MLevel* level = MEngine::getInstance()->getLevel();
    MOText* text = level->getCurrentScene()->getTextByName(window.name_edit->value());

    if(!text)
    {
        MLOG_ERROR("Text field does not exist!");
        return;
    }

    text->setText(window.text_text_edit->buffer()->text());
    text->setSize(window.text_size_edit->value());

    MVector4 color;
    color.x = window.text_r->value();
    color.y = window.text_g->value();
    color.z = window.text_b->value();
    color.w = window.text_a->value();

    text->setColor(color);

    if(!text_load_font(window.text_font_edit->value(), text))
    {
        window.text_font_edit->value(text->getFontRef()->getFilename());
        fl_alert("Could not load font!");
    }

    window.glbox->redraw();
}

void text_alignment_callback(Fl_Menu_*, long value)
{
    MLevel* level = MEngine::getInstance()->getLevel();
    MOText* text = level->getCurrentScene()->getTextByName(window.name_edit->value());

    if(!text)
    {
        MLOG_ERROR("Text field does not exist!");
        return;
    }

    text->setAlign((M_ALIGN_MODES) value);
    window.glbox->redraw();
}

void choose_text_color(Fl_Button*, void*)
{
    double r = window.text_r->value();
    double g = window.text_g->value();
    double b = window.text_b->value();

    fl_color_chooser("Choose a color", r, g, b);

    window.text_r->value(r);
    window.text_g->value(g);
    window.text_b->value(b);

    edit_text_properties((Fl_Widget*) NULL, NULL);
}

void add_text_callback(Fl_Menu_ *, void *)
{
    const char* filename = fl_native_file_chooser("Choose font", "*.ttf", (current_project.path + "fonts").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(!filename)
        return;

    Maratis* maratis = Maratis::getInstance();
    maratis->autoSave();

    maratis->okAddFont(filename);

    window.glbox->redraw();
	update_scene_tree();
}

void add_camera_callback(Fl_Menu_*,void*)
{
    Maratis::getInstance()->addCamera();
    window.glbox->redraw();
	update_scene_tree();
}

bool str_starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void play_game_callback(Fl_Menu_*, void*)
{
    if(current_project.path.empty())
    {
        fl_alert("Can't run the game: You first have to open a project!");
        return;
    }

    save_level_callback(NULL, 0);

#ifndef WIN32
    std::string project_name = current_project.file_path.substr(current_project.file_path.find_last_of("/")+1);
    project_name = project_name.erase(project_name.find_last_of("."));

    FILE* file = popen((current_project.path + "MaratisPlayer \"" + project_name + "\" 1024 768 0 1").c_str(), "r");
#else
    FILE* file = _popen((current_project.path + "MaratisPlayer.exe").c_str(), "r");
#endif

    size_t size = 256;
    char* line = (char*) malloc(size);

    double frametime;
    double framecount;
    int framemax = 0;
    int framemin = -1;
    char str[20];

    while(getline(&line, &size, file) > 0)
    {
        if(str_starts_with("profiler", line))
        {
            double time;
            sscanf(line, "%*s %s %lf", str, &time);

            if(!strcmp(str, "frametime"))
            {
                frametime += time;
                framecount++;

                if(time > framemax)
                    framemax = time;

                if(time < framemin || framemin < 0)
                    framemin = time;
            }
        }
        else
            printf("%s", line);
    }

    double avFrame = frametime / framecount;
    MLOG_INFO("Number of frames: " << framecount << " Average frametime: " << avFrame << "ms Average framerate: " << 1000/avFrame);
    MLOG_INFO("Max frametime: " << framemax << "ms Min frametime: " << framemin << "ms");
    fclose(file);
    free(line);

    update_scene_tree();
}

void add_behavior_menu_callback(Fl_Menu_* menu, const char* name)
{
    MLOG_INFO("Adding behavior to object");
    MEngine* engine = MEngine::getInstance();
    MBehaviorManager* manager = engine->getBehaviorManager();
    MObject3d* object = engine->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object)
    {
        MLOG_ERROR("Can't add behavior: Object does not exist!");
        return;
    }

    MBehaviorCreator* behavior = manager->getBehaviorByName(name);

    if(!behavior)
    {
        MLOG_ERROR("Can't add behavior: Behavior does not exist!");
        return;
    }

    if((behavior->getObjectFilter() != object->getType()) && (behavior->getObjectFilter() != M_OBJECT3D))
    {
        MLOG_ERROR("Can't add behavior: Behavior is from a different type!");
        fl_alert("Could not add behavior: Behavior is from a different type than the object!");
        return;
    }

    object->addBehavior(behavior->getNewBehavior(object));
    update_scene_tree();
    create_behavior_ui(object);
}

void update_behavior_menu()
{
    window.behavior_menu->clear();
    MEngine* engine = MEngine::getInstance();
    MBehaviorManager* manager = engine->getBehaviorManager();

    for(int i = 0; i < manager->getBehaviorsNumber(); i++)
    {
        MBehaviorCreator* behavior = manager->getBehaviorByIndex(i);

        std::string name = behavior->getName();
        name = "Add/Add Behavior/" + name;
        window.menu_bar->add(name.c_str(), 0, (Fl_Callback*) add_behavior_menu_callback, (void*) behavior->getName(), 0);
    }
}

void import_mesh_callback(Fl_Menu_*, void*)
{
    const char* filename = fl_native_file_chooser("Choose file", "*.obj\n*.dae\n*.3ds\n*.b3d",
                                                  current_project.path.c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(!filename)
        return;

    M_importAssimpMeshes(filename);
    update_scene_tree();
    window.glbox->redraw();
}

void delete_object_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->deleteSelectedObjects();

    update_scene_tree();
    window.glbox->redraw();
}

void undo_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->undo();

    update_scene_tree();
    window.glbox->redraw();
}

void redo_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->redo();

    update_scene_tree();
    window.glbox->redraw();
}

void add_sound_callback(Fl_Menu_ *, void *)
{
    const char* filename = fl_native_file_chooser("Choose file", "*.wav, *.ogg", (current_project.path + "sounds").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(!filename)
        return;

    Maratis::getInstance()->okAddSound(filename);

    update_scene_tree();
    window.glbox->redraw();
}

void edit_sound_callback(Fl_Value_Input*, void*)
{
    MOSound* sound = MEngine::getInstance()->getLevel()->getCurrentScene()->getSoundByName(window.name_edit->value());

    if(!sound)
        return;

    sound->setPitch(window.sound_pitch_edit->value());
    sound->setGain(window.sound_gain_edit->value());
    sound->setRadius(window.sound_radius_edit->value());
    sound->setRolloff(window.sound_rolloff_edit->value());
}

void edit_sound_chk_btn(Fl_Check_Button*, void*)
{
    MOSound* sound = MEngine::getInstance()->getLevel()->getCurrentScene()->getSoundByName(window.name_edit->value());

    if(!sound)
        return;

    sound->setLooping(window.sound_loop_button->value());
    sound->setRelative(window.sound_relative_button->value());
}

void add_scene_callback(Fl_Menu_ *, void *)
{
    Fl_Window* win = window.create_scene_window();
    win->show();

    while(win->shown())
        Fl::wait();

    if(window.scene_name.empty())
        return;

    Maratis::getInstance()->autoSave();
    MLevel* level = MEngine::getInstance()->getLevel();
    MScene* scene = level->addNewScene();
    scene->setName(window.scene_name.c_str());

    unsigned int index = level->getScenesNumber()-1;

    change_scene_callback(NULL, index);
    update_scene_tree();
}

void scene_setup_callback(Fl_Menu_ *, void*)
{
    SceneSetupDlg dlg;
    Fl_Window* win = dlg.create_window();

    MScene* scene = MEngine::getInstance()->getLevel()->getCurrentScene();
    dlg.lua_script_edit->value(scene->getScriptFilename());
    dlg.scene_name_edit->value(scene->getName());

    win->show();
    while(win->shown())
        Fl::wait();

    if(strcmp(win->label(), "Success"))
    {
        MLOG_INFO("Won't change scene.");
        return;
    }

    Maratis::getInstance()->autoSave();
    scene->setName(dlg.scene_name_edit->value());
    scene->setScriptFilename(dlg.lua_script_edit->value());

    update_scene_tree();
}

void delete_scene_callback(Fl_Menu_ *, void *)
{
    MLevel* level = MEngine::getInstance()->getLevel();

    if(level->getScenesNumber()-1 <= 0)
    {
        MLOG_INFO("Can not delete scene! The level needs at least one scene!");
        fl_alert("Can not delete scene! The level needs at least one scene!");
        return;
    }

    Maratis::getInstance()->autoSave();
    level->deleteScene(level->getCurrentSceneId());

    Maratis::getInstance()->clearSelectedObjects();
    update_scene_tree();
    window.glbox->redraw();
}

void duplicate_object_callback(Fl_Menu_ *, void *)
{
    Maratis::getInstance()->duplicateSelectedObjects();

    update_scene_tree();
    window.glbox->redraw();
}

// TODO: Real about dialog
void about_menu_callback(Fl_Menu_*, void*)
{
    fl_alert("This program is distributed under the terms of the GNU General Public License.");
}

void select_all_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->selectAll();
    update_scene_tree();
    window.glbox->redraw();
}

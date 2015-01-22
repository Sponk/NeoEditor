#include <HTTPRequest.h>

#include "Callbacks.h"
#include <cstdio>
#include <cerrno>

#ifndef WIN32
#include <unistd.h>
#endif

#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Tree_Item.H>
#include <FL/Fl_Tree_Prefs.H>
#include <FL/Fl_PNG_Image.H>

#include <NeoEngine.h>
#include <MWindow.h>
#include <ImageLoader.h>
#include <LevelLoad.h>
#include <LevelSave.h>
#include <MCore.h>

#include <GuiSystem.h>

using namespace Neo;

#include "../MFilesUpdate/MFilesUpdate.h"
#include "../MLoaders/MAssimpMeshLoader.h"
#include <MCore.h>
#include "MainWindow.h"
#include "Translator.h"
#include <Shiny.h>

#include <SDLThread.h>

open_project_t current_project;
bool reload_editor = false;

extern EditorWindow window;

Fl_Window* object_window;
Fl_Window* light_window;
Fl_Window* text_window;
Fl_Window* sound_window;
Fl_Window* camera_window;

// TODO: Configurable!
Fl_PNG_Image camera_icon("gui/icons/camera.png");
Fl_PNG_Image entity_icon("gui/icons/entity.png");
Fl_PNG_Image light_icon("gui/icons/light.png");
Fl_PNG_Image text_icon("gui/icons/text.png");
Fl_PNG_Image sound_icon("gui/icons/sound.png");

Fl_Text_Buffer textbuf;

PlayerConsole pluginConsole;
Fl_Text_Buffer pluginOutput;

PlayerConsole console;
Fl_Text_Buffer console_buffer;

char labels[][2] = {{"X"},{"Y"},{"Z"},{"W"}};

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

/*
 * Replacement for fl_ask
 */
int ask(const char* text)
{
    return fl_choice("%s", "No", "Yes", NULL, text);
}

MVector3 flColorToVector(int c)
{
    char unsigned bytes[4];
    bytes[0] = (c >> 24) & 0xFF;
    bytes[1] = (c >> 16) & 0xFF;
    bytes[2] = (c >> 8) & 0xFF;
    bytes[3] = c & 0xFF;

    return MVector3(static_cast<float>(bytes[0])/255.0f, static_cast<float>(bytes[1])/255.0f, static_cast<float>(bytes[2])/255.0f);
}

void save_settings()
{
    // Save settings
    #ifndef WIN32
        std::string fullpath = getenv("HOME");
        fullpath += "/.neoeditor/";
    #else
        std::string fullpath = getenv("APPDATA");
        fullpath += "\\neoeditor\\";
    #endif

    char dir[256];
    getGlobalFilename(dir, fullpath.c_str(), "config.ini");

    if(!isFileExist(fullpath.c_str()))
        createDirectory(fullpath.c_str());

    MLOG_INFO("Saving settings to: " << dir);

    ofstream out(dir);
    if(out)
    {
        if(window.inputMethod != NULL)
        {
            out << "[input]" << endl;
            out << "inputMethod=" << window.inputMethod->getName() << endl;
        }

        out << "[theme]" << endl;

        if(Fl::scheme() != NULL)
            out << "scheme=" << Fl::scheme() << endl;
        else
            out << "scheme=none" << endl;

        MVector3 vector = flColorToVector(Fl::get_color(FL_BACKGROUND_COLOR));
        out << "background_r=" << vector.x << endl;
        out << "background_g=" << vector.y << endl;
        out << "background_b=" << vector.z << endl;

        vector = flColorToVector(Fl::get_color(FL_BACKGROUND2_COLOR));
        out << "background2_r=" << vector.x << endl;
        out << "background2_g=" << vector.y << endl;
        out << "background2_b=" << vector.z << endl;

        vector = flColorToVector(Fl::get_color(FL_FOREGROUND_COLOR));
        out << "foreground_r=" << vector.x << endl;
        out << "foreground_g=" << vector.y << endl;
        out << "foreground_b=" << vector.z << endl;

        out << "[window]" << endl;
        out << "xpos=" << main_window->x_root() << endl;
        out << "ypos=" << main_window->y_root() << endl;
        out << "width=" << main_window->w() << endl;
        out << "height=" << main_window->h() << endl;
        out << "translationSpeed=" << translation_speed << endl;
        out << "rotationSpeed=" << rotation_speed << endl;

        out.close();
    }

    getGlobalFilename(dir, fullpath.c_str(), "language.ini");
    out.open(dir, ios::out);

    MLOG_INFO("Writing language settings to: " << dir);

    if(out)
    {
        out << "[lang]" << endl << "name=" << Translator::getInstance()->getLanguageFile();
        out.close();
    }
}

void quit_callback(Fl_Menu_*, void*)
{
    if(ask(tr("Do you really want to exit?")))
    {
        // Saving settings
        save_settings();
        // Quitting engine
        NeoEngine::getInstance()->setActive(false);
        exit(0);
    }
}

void window_quit(Fl_Double_Window*, void*)
{
    quit_callback(NULL, NULL);
}

void add_to_tree(Object3d* entity, std::string path)
{
    Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

    if(entity)
    {
        path += "/" + std::string(entity->getName());

        if(!window.scene_tree->find_item(path.c_str()))
        {
            Fl_Tree_Item* item = window.scene_tree->add(path.c_str());

            // Assign icons
            switch(entity->getType())
            {
            case M_OBJECT3D_CAMERA:
            		item->usericon(&camera_icon);
            	break;
            case M_OBJECT3D_LIGHT:
            		item->usericon(&light_icon);
            	break;
            case M_OBJECT3D_ENTITY:
            		item->usericon(&entity_icon);
            	break;
            case M_OBJECT3D_TEXT:
            		item->usericon(&text_icon);
            	break;
            case M_OBJECT3D_SOUND:
            		item->usericon(&sound_icon);
            	break;
            }
        }

        for(int i = 0; i < entity->getChildsNumber(); i++)
        {
        	Object3d* object = entity->getChild(i);
            if(!window.scene_tree->find_item((path + "/" + std::string(object->getName())).c_str()))
            {
            	Fl_Tree_Item* item = window.scene_tree->add((path + "/" + std::string(object->getName())).c_str());

                // Assign icons
                switch(entity->getType())
                {
                case M_OBJECT3D_CAMERA:
                		item->usericon(&camera_icon);
                	break;
                case M_OBJECT3D_LIGHT:
                		item->usericon(&light_icon);
                	break;
                case M_OBJECT3D_ENTITY:
                		item->usericon(&entity_icon);
                	break;
                case M_OBJECT3D_TEXT:
                		item->usericon(&text_icon);
                	break;
                case M_OBJECT3D_SOUND:
                		item->usericon(&sound_icon);
                	break;
                }
            }

            add_to_tree(scene->getObjectByName(entity->getChild(i)->getName()), path.c_str());
        }
    }
}

void change_scene_callback(Fl_Menu_*, long index)
{
    // MLOG_INFO("Changing to scene " << index);

    Level* level = NeoEngine::getInstance()->getLevel();
    Maratis::getInstance()->autoSave();
    level->setCurrentSceneId(index);
    Maratis::getInstance()->clearSelectedObjects();

    update_scene_tree();
}

void update_scene_tree()
{
    window.scene_tree->clear();

    Level* level = NeoEngine::getInstance()->getLevel();
    Scene* scene = level->getCurrentScene();

    unsigned int index = 0;

    window.scene_tree->begin();
    for(int i = 0; i < scene->getObjectsNumber(); i++)
    {
        Object3d* entity = scene->getObjectByIndex(i);
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
    const char* filename = fl_native_file_chooser(tr("Open level"), "*.level", (current_project.path + "/levels").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(filename)
    {
        Maratis::getInstance()->loadLevel(filename);

        update_scene_tree();
        current_project.level = filename;
    }

    reload_editor = true;
    window.scene_tree->redraw();
}

void open_project_callback(Fl_Menu_*, void*)
{
    if(!current_project.path.empty() && !ask(tr("You need to close the current project. Do you want to proceed?")))
        return;

    const char* home = NULL;

#ifndef WIN32
    home = getenv("HOME");
#else
    home = getenv("USERPROFILE");
#endif

    const char* filename = fl_native_file_chooser(tr("Open project"), "*.mproj", home, Fl_Native_File_Chooser::BROWSE_FILE);

    if(filename)
    {
        // Show the loading dialog
        WaitDlg dlg;
        Fl_Double_Window* win = dlg.create_window();
        win->show();

        // Somehow needs 4 Fl::wait calls for the window to actually appear
        // TODO: Own thread!
        Fl::wait();
        Fl::wait();
        Fl::wait();
        Fl::wait();

        // Actually load the project
        current_project.changed = false;
        current_project.path = filename;
        current_project.file_path = filename;

#ifndef WIN32
        current_project.path = current_project.path.erase(current_project.path.find_last_of("/")+1, current_project.path.length());
#else
        current_project.path = current_project.path.erase(current_project.path.find_last_of("\\")+1, current_project.path.length());
#endif
        NeoGame* game = NeoEngine::getInstance()->getGame();
        Maratis::getInstance()->loadProject(filename);

        ::window.glbox->loadPostEffectsFromGame(game);

        current_project.level = Maratis::getInstance()->getCurrentLevel();
        update_scene_tree();

        // Destroy the dialog
        Fl::delete_widget(win);
    }

    reload_editor = true;
    window.scene_tree->redraw();
}

float rotation_speed = 1.0;

void rotation_speed_callback(Fl_Value_Input* input, void*)
{
    rotation_speed = input->value();
}

float translation_speed = 1.0;

void translation_speed_callback(Fl_Value_Input* input, void*)
{
    translation_speed = input->value();
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
}

void remove_window(Fl_Window* win)
{
    if(win)
    {
        win->hide();
        Fl::delete_widget(win);
    }
}

void* get_variable_pointer(const char* bname, const char* vname, Object3d* object)
{
    if(!vname || !bname || !object)
        return NULL;

    Behavior* behavior;
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
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    *((float*) get_variable_pointer(data, input->label(), object)) = input->value();
}

void behavior_int_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    *(int*) get_variable_pointer(data, input->label(), object) = input->value();
}

void behavior_bool_callback(Fl_Check_Button* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    *(bool*) get_variable_pointer(data, input->label(), object) = input->value();
}

void behavior_string_callback(Fl_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object || !input || !data)
        return;

    ((MString*) get_variable_pointer(data, input->label(), object))->set(input->value());
    window.m_deferredUiUpdate = true;
}

void behavior_vector4_callback(Fl_Value_Input* input, const char* data)
{
    //MLOG_INFO("Changed " << input->label() << " " << data);
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

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
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

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
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

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

void remove_behavior(Fl_Button*, long behavior)
{
    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object)
        return;

    Maratis::getInstance()->autoSave();
    object->deleteBehavior(behavior);

    update_scene_tree();
    create_behavior_ui(object);
    window.behaviors_scroll->redraw();
}

void create_behavior_ui(Object3d* object)
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

        Behavior* behavior = object->getBehavior(b);
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
            NeoVariable var = behavior->getVariable(i);

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

void scene_tree_callback(DnDTree* tree, long update_tree)
{
    if(update_tree)
    {
        update_scene_tree();
        return;
    }

    Fl_Tree_Item* item = tree->first_selected_item();

    if(!item)
        return;

    const char* name = item->label();

    if(!name || !strcmp(name, "ROOT"))
        return;

    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(name);

    if(!object)
    {
        MLOG_ERROR("Object does not exist!");
        return;
    }

    Maratis* maratis = Maratis::getInstance();

    //if(item == tree->find_clicked())
    //{
    	maratis->clearSelectedObjects();
    	maratis->addSelectedObject(object);

    	Fl_Tree_Item* t = item;
    	while((t = t->next()) != NULL)
    	{
    		if(t->is_selected() == 0)
    			continue;

    	    const char* name = t->label();

    	    if(!name || !strcmp(name, "ROOT"))
    	        continue;

    	    Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(name);

    	    if(object)
    	    	maratis->addSelectedObject(object);
    	}
    //}

    // TODO: Testing!
    if(Fl::event_clicks() && (tree == Fl::focus() || window.glbox == Fl::focus()))
    {
        /*MOCamera* vue = Maratis::getInstance()->getPerspectiveVue();
        vue->setPosition(object->getPosition());
        vue->updateMatrix();
        window.glbox->redraw();*/

        Maratis::getInstance()->focusSelection();
        Maratis::getInstance()->getPerspectiveVue()->updateMatrix();
    }

    MVector3 position = object->getPosition();
    MVector3 rotation = object->getEulerRotation();
    MVector3 scale = object->getScale();

    if(update_name)
        create_behavior_ui(object);

    window.special_tab->begin();

    if(object->getType() == M_OBJECT3D_ENTITY)
    {
        DELETE_WINDOW(text_window);
        DELETE_WINDOW(light_window);
        DELETE_WINDOW(sound_window);
        DELETE_WINDOW(camera_window);

        if(object_window == NULL)
        {
            object_window = window.create_object_window();
            object_window->position(window.special_tab->x()+3, window.special_tab->y()+3);

            window.special_tab->add(object_window);
        }

        OEntity* entity = (OEntity*) object;
        window.object_mesh_edit->value(entity->getMeshRef()->getFilename());

        if(update_name)
        {
            window.object_invisible_button->value(!entity->isVisible());
			window.object_shadow_button->value(entity->hasShadow());
            window.object_occluder_button->value(entity->isOccluder());
        }

        PhysicsProperties* phys = entity->getPhysicsProperties();

        if(phys)
        {
            window.object_mass_edit->value(phys->getMass());
            window.object_fric_edit->value(phys->getFriction());
            window.object_rest_edit->value(phys->getRestitution());

            window.object_angular_damping_edit->value(phys->getAngularDamping());
            window.object_linear_damping_edit->value(phys->getLinearDamping());

            MVector3 linear_factor = *phys->getLinearFactor();
            window.xlinear_edit->value(linear_factor.x);
            window.ylinear_edit->value(linear_factor.y);
            window.zlinear_edit->value(linear_factor.z);

            window.object_angular_factor_edit->value(phys->getAngularFactor());          

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
        OLight* light = (OLight*) object;

        DELETE_WINDOW(text_window);
        DELETE_WINDOW(sound_window);
        DELETE_WINDOW(object_window);
        DELETE_WINDOW(camera_window);

        if(light_window == NULL)
        {
            light_window = window.create_light_window();

            light_window->position(window.special_tab->x()+3, window.special_tab->y());
	    light_window->size(window.special_tab->w(), window.special_tab->h());
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

            camera_window->position(window.special_tab->x()+3, window.special_tab->y());
            window.special_tab->add(camera_window);
        }

        OCamera* camera = (OCamera*) object;
        MVector3 clearColor = camera->getClearColor();
        MVector3 fogColor = camera->getFogColor();

        window.camera_color_r->value(clearColor.x);
        window.camera_color_g->value(clearColor.y);
        window.camera_color_b->value(clearColor.z);

        window.fog_color_r->value(fogColor.x);
        window.fog_color_g->value(fogColor.y);
        window.fog_color_b->value(fogColor.z);

        window.camera_fov_edit->value(camera->getFov());
        window.camera_clipping_near_edit->value(camera->getClippingNear());
        window.camera_clipping_far_edit->value(camera->getClippingFar());

        window.camera_fog_distance_edit->value(camera->getFogDistance());

        if(update_name)
        {
            window.camera_skybox_edit->value(camera->getSkybox()->getPath());
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

            text_window->position(window.special_tab->x()+3, window.special_tab->y());
            window.special_tab->add(text_window);
        }

        OText* text = (OText*) object;

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

            sound_window->position(window.special_tab->x()+3, window.special_tab->y());
            window.special_tab->add(sound_window);
        }

        OSound* sound = (OSound*) object;

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
        Object3d* parent = object->getParent();

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

    window.special_tab->end();
    window.special_tab->redraw();
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
                Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
                Object3d* entity = scene->getObjectByName(name);
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
                Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
                Object3d* entity = scene->getObjectByName(name);

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
                    Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
                    Object3d* entity = scene->getObjectByName(name);

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
}

void edit_name_callback(Fl_Input*, void*)
{
    if(Maratis::getInstance()->getSelectedObjectsNumber() == 0)
        return;

    Object3d* object = Maratis::getInstance()->getSelectedObjectByIndex(0);
    object->setName(window.name_edit->value());

    Object3d* parent = object->getParent();

    if(parent == NULL && !strcmp(window.parent_edit->value(), "none"))
    {
        update_scene_tree();
        return;
    }
    else if(parent == NULL)
    {
        parent = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(window.parent_edit->value());

        if(!parent)
        {
            MLOG_ERROR("Can't set parent: Parent object not found!");
            fl_alert(tr("Can't set parent: Parent object not found!"));

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
    OLight* light = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getLightByName(window.name_edit->value());

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
	
    window.special_tab->redraw();
}

void edit_light_properties_chk_btn(Fl_Check_Button * button, void *)
{    
    OLight* light = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getLightByName(window.name_edit->value());

    if(window.light_spot_button->value() == 1)
    {
        // Should 45 be the default value?
        if(window.spot_angle_edit->value() > 90)
            window.spot_angle_edit->value(45);

        light->setSpotAngle(window.spot_angle_edit->value());
    }
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
}

void choose_light_color(Fl_Button*, void*)
{
    double r = window.light_color_r->value();
    double g = window.light_color_g->value();
    double b = window.light_color_b->value();

    fl_color_chooser(tr("Choose a color"), r, g, b);

    window.light_color_r->value(r);
    window.light_color_g->value(g);
    window.light_color_b->value(b);

    edit_light_properties(NULL, NULL);
}

void choose_camera_color(Fl_Button*, void*)
{
	double r = window.camera_color_r->value();
	double g = window.camera_color_g->value();
	double b = window.camera_color_b->value();

    fl_color_chooser(tr("Choose a color"), r, g, b);

	window.camera_color_r->value(r);
	window.camera_color_g->value(g);
	window.camera_color_b->value(b);

    edit_camera_properties(NULL, NULL);
}

void choose_fog_color(Fl_Button*, void*)
{
    double r = window.fog_color_r->value();
    double g = window.fog_color_g->value();
    double b = window.fog_color_b->value();

    fl_color_chooser(tr("Choose a color"), r, g, b);

    window.fog_color_r->value(r);
    window.fog_color_g->value(g);
    window.fog_color_b->value(b);

    edit_camera_properties(NULL, NULL);
}

void publish_callback(Fl_Menu_*, void*)
{
    if(current_project.path.empty())
    {
        MLOG_ERROR("No project open!");
        fl_message(tr("Can't publish: You need to create/open a project first!"));
        return;
    }

    PublishDlg* dlg = new PublishDlg;
    dlg->create_window()->show();
}

void save_level_callback(Fl_Menu_ *, long mode)
{
    const char* filename = NULL;
    switch(mode)
    {
    // Save
    case 0:
        if(current_project.level.empty())
        {
            filename = fl_native_file_chooser(tr("Select a file to save"), "*.level",  (current_project.path + "levels").c_str(),
                                              Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

            if(filename == NULL)
                return;

            current_project.level = filename;
        }

        Maratis::getInstance()->setCurrentLevel(current_project.level.c_str());

        break;
    // Save as
    case 1:
        filename = fl_native_file_chooser(tr("Select a file to save"), "*.level", (current_project.path + "levels").c_str(),
                                          Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

        if(filename == NULL)
            return;

        current_project.level = filename;
        Maratis::getInstance()->setCurrentLevel(current_project.level.c_str());

        break;
    }

    NeoGame* game = NeoEngine::getInstance()->getGame();
    game->getPostProcessor()->setShaderPath(::window.glbox->getPostProcessor()->getVertexShader(), ::window.glbox->getPostProcessor()->getFragmentShader());

    Maratis::getInstance()->save();
}

void new_project_callback(Fl_Menu_*, void*)
{
    const char* filename = fl_native_file_chooser(tr("Select project file"), "*.mproj", NULL, Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

    if(!filename)
        return;

    current_project.level = "";

    current_project.changed = false;
    current_project.path = filename;
    current_project.file_path = filename;

#ifndef WIN32
    current_project.path = current_project.path.erase(current_project.path.find_last_of("/")+1, current_project.path.length());
#else
    current_project.path = current_project.path.erase(current_project.path.find_last_of("\\")+1, current_project.path.length());
#endif

    Maratis::getInstance()->okNewProject(filename);
    update_scene_tree();
}

void new_level_callback(Fl_Menu_*, void*)
{
    current_project.level = "";

    Maratis::getInstance()->newLevel();
    update_scene_tree();
    //main_window->label("Untitled level");
}

void edit_shape_callback(Fl_Menu_* menu, long type)
{
    OEntity * entity = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(window.name_edit->value());

    if(!entity || !entity->getPhysicsProperties())
        return;

    entity->getPhysicsProperties()->setCollisionShape((COLLISION_SHAPE_TYPE)type);
}

void edit_object_chk_btn(Fl_Check_Button*, void*)
{
    OEntity * entity = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(window.name_edit->value());

    if(!entity)
        return;

    entity->setInvisible(window.object_invisible_button->value());
	entity->enableShadow(window.object_shadow_button->value());
    entity->enableOccluder(window.object_occluder_button->value());

    PhysicsProperties* phys = entity->getPhysicsProperties();

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
}

void edit_object_properties(Fl_Value_Input*, void*)
{
    OEntity * entity = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getEntityByName(window.name_edit->value());

    if(!entity)
        return;

    PhysicsProperties* phys = entity->getPhysicsProperties();

    if(!phys)
        return;

    phys->setMass(window.object_mass_edit->value());
    phys->setFriction(window.object_fric_edit->value());
    phys->setRestitution(window.object_rest_edit->value());

    phys->setAngularDamping(window.object_angular_damping_edit->value());
    phys->setLinearDamping(window.object_linear_damping_edit->value());

    phys->setLinearFactor(MVector3(window.xlinear_edit->value(), window.ylinear_edit->value(), window.zlinear_edit->value()));
    phys->setAngularFactor(window.object_angular_factor_edit->value());
}

void add_mesh_callback(Fl_Menu_*, void*)
{
    if(current_project.level.empty())
    {
        fl_message(tr("You need to save the level before adding meshes!"));
        return;
    }

    const char* filename = fl_native_file_chooser(tr("Choose mesh"), "*.mesh", (current_project.path + "meshs").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(filename)
    {
        Maratis::getInstance()->autoSave();
        Maratis::getInstance()->okAddEntity(filename);
        update_scene_tree();
    }

	update_scene_tree();
}

void add_light_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->autoSave();
    Maratis::getInstance()->addLight();
    update_scene_tree();
}

void edit_camera_properties(Fl_Value_Input*, void*)
{
    OCamera* camera = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getCameraByName(window.name_edit->value());

    if(!camera)
    {
        MLOG_ERROR("Camera object does not exist!");
        return;
    }

    camera->setClearColor(MVector3(window.camera_color_r->value(), window.camera_color_g->value(), window.camera_color_b->value()));
    camera->setFogColor(MVector3(window.fog_color_r->value(), window.fog_color_g->value(), window.fog_color_b->value()));

    camera->setFov(window.camera_fov_edit->value());
    camera->setClippingNear(window.camera_clipping_near_edit->value());
    camera->setClippingFar(window.camera_clipping_far_edit->value());

    camera->setFogDistance(window.camera_fog_distance_edit->value());
}

void edit_camera_skybox(Fl_Input*, void*)
{
    OCamera* camera = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getCameraByName(window.name_edit->value());

    if(!camera)
    {
        MLOG_ERROR("Camera object does not exist!");
        return;
    }
    camera->loadSkybox(window.camera_skybox_edit->value());
}

void edit_camera_properties_chk_btn(Fl_Check_Button*, void*)
{
    OCamera* camera = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getCameraByName(window.name_edit->value());

    if(!camera)
    {
        MLOG_ERROR("Camera object does not exist!");
        return;
    }

    camera->enableOrtho(window.camera_ortho_button->value());
    camera->enableFog(window.camera_fog_button->value());
}

bool text_load_font(const char* name, OText* text)
{
    if(!name || !text)
        return false;

    Level* level = NeoEngine::getInstance()->getLevel();
    FontRef* ref = text->getFontRef();

    if(!strcmp(name, ref->getFilename()))
        return true;

    FontRef* fontRef = level->loadFont(name);

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
    const char* filename = fl_native_file_chooser(tr("Choose font"), "*.ttf", (current_project.path + "fonts").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);
    Level* level = NeoEngine::getInstance()->getLevel();
    OText* text = level->getCurrentScene()->getTextByName(window.name_edit->value());

    if(!text)
    {
        MLOG_ERROR("Text field does not exist!");
        return;
    }

    if(!filename)
        return;

    if(!text_load_font(filename, text))
    {
        fl_alert(tr("Could not load font!"));
        return;
    }

    window.text_font_edit->value(filename);
}

void edit_text_properties(Fl_Widget*, void*)
{
    Level* level = NeoEngine::getInstance()->getLevel();
    OText* text = level->getCurrentScene()->getTextByName(window.name_edit->value());

    if(!text)
    {
        MLOG_ERROR(tr("Text field does not exist!"));
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
        fl_alert(tr("Could not load font!"));
    }
}

void text_alignment_callback(Fl_Menu_*, long value)
{
    Level* level = NeoEngine::getInstance()->getLevel();
    OText* text = level->getCurrentScene()->getTextByName(window.name_edit->value());

    if(!text)
    {
        MLOG_ERROR("Text field does not exist!");
        return;
    }

    text->setAlign((TEXT_ALIGN_MODES) value);
}

void choose_text_color(Fl_Button*, void*)
{
    double r = window.text_r->value();
    double g = window.text_g->value();
    double b = window.text_b->value();

    fl_color_chooser(tr("Choose a color"), r, g, b);

    window.text_r->value(r);
    window.text_g->value(g);
    window.text_b->value(b);

    edit_text_properties((Fl_Widget*) NULL, NULL);
}

void add_text_callback(Fl_Menu_ *, void *)
{
    if(current_project.level.empty())
    {
        fl_message(tr("You need to save the level before adding objects!"));
        return;
    }

    const char* filename = fl_native_file_chooser(tr("Choose font"), "*.ttf", (current_project.path + "fonts").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(!filename)
        return;

    Maratis* maratis = Maratis::getInstance();
    maratis->autoSave();

    maratis->okAddFont(filename);
	update_scene_tree();
}

void add_camera_callback(Fl_Menu_*,void*)
{
    Maratis::getInstance()->addCamera();
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
        fl_alert(tr("Can't run the game: You first have to open a project!"));
        return;
    }

    save_level_callback(NULL, 0);
	    
#ifndef WIN32
    std::string project_name = current_project.file_path.substr(current_project.file_path.find_last_of("/")+1);
    project_name = project_name.erase(project_name.find_last_of("."));

    if(!isFileExist((current_project.path + "NeoPlayer").c_str()))
    {
        MLOG_ERROR("Could not start player! Executable does not exist!");
        fl_alert(tr("Could not start player! Make sure that the executable exists and you have rights to start it.\nIf it does not exist, try the 'Update player' option in the project menu."));
        return;
    }

    errno = 0;
    FILE* file = popen(("\"" + current_project.path + "NeoPlayer\" \"" + project_name + "\" 1024 768 0 1 2>&1").c_str(), "r");
#else
	std::string project_name = current_project.file_path.substr(current_project.file_path.find_last_of("\\")+1);
    project_name = project_name.erase(project_name.find_last_of("."));

    if(!isFileExist((current_project.path + "NeoPlayer.exe").c_str()))
    {
        MLOG_ERROR("Could not start player! Executable does not exist!");
        fl_alert(tr("Could not start player! Make sure that the executable exists and you have rights to start it.\nIf it does not exist, try the 'Update player' option in the project menu."));
        return;
    }

    errno = 0;
    FILE* file = _popen(("\"" + current_project.path + "NeoPlayer.exe '" + project_name + "' 1024 768 0 1\" 2>&1").c_str(), "r");
#endif

    if(file == NULL || errno != 0)
	{
		MLOG_ERROR("Could not start player!");
        fl_alert(tr("Could not start player! Make sure that the executable exists and you have rights to start it.\nIf it does not exist, try the 'Update player' option in the project menu."));
		return;
	}

    if(console.closed)
    {
        Fl_Window* window = console.create_window();
        window->label(tr("Player Console"));
        window->show();
    }

	console.output_edit->buffer(&console_buffer);
    console_buffer.text("");

	Fl::wait();

    size_t size = 256;
    char* line = (char*) malloc(size+1);
    Fl::wait();

    // TODO: Do all profiling on the client side!
    while(getline(&line, &size, file) > 0)
    {
		console_buffer.append(line);
		console.output_edit->move_down();
		console.output_edit->show_insert_position();
        Fl::wait();

        printf("%s", line);
    }
	    
#ifndef WIN32
    pclose(file);
#else
	_pclose(file);
#endif

	free(line);
    update_scene_tree();
}

void play_game_button_callback(Fl_Button*, void*)
{
    play_game_callback(NULL, NULL);
}

void add_behavior_menu_callback(Fl_Menu_* menu, const char* name)
{
    NeoEngine* engine = NeoEngine::getInstance();
    BehaviorManager* manager = engine->getBehaviorManager();
    Object3d* object = engine->getLevel()->getCurrentScene()->getObjectByName(window.name_edit->value());

    if(!object)
    {
        MLOG_ERROR("Can't add behavior: Object does not exist!");
        return;
    }

    BehaviorCreator* behavior = manager->getBehaviorByName(name);

    if(!behavior)
    {
        MLOG_ERROR("Can't add behavior: Behavior does not exist!");
        return;
    }

    if((behavior->getObjectFilter() != object->getType()) && (behavior->getObjectFilter() != M_OBJECT3D))
    {
        MLOG_ERROR("Can't add behavior: Behavior is from a different type!");
        fl_alert(tr("Could not add behavior: Behavior is from a different type than the object!"));
        return;
    }

    object->addBehavior(behavior->getNewBehavior(object));
    update_scene_tree();
    create_behavior_ui(object);
}

void update_behavior_menu()
{
    window.behavior_menu->clear();
    NeoEngine* engine = NeoEngine::getInstance();
    BehaviorManager* manager = engine->getBehaviorManager();

    for(int i = 0; i < manager->getBehaviorsNumber(); i++)
    {
        BehaviorCreator* behavior = manager->getBehaviorByIndex(i);

        std::string name = behavior->getName();
        name = string(tr("Add")) + "/" + tr("Add Behavior") + "/" + name;
        window.menu_bar->add(name.c_str(), 0, (Fl_Callback*) add_behavior_menu_callback, (void*) behavior->getName(), 0);
    }
}

void import_mesh_callback(Fl_Menu_*, void*)
{
    const char* filename = fl_native_file_chooser(tr("Choose file"), "*.{obj,dae,3ds,b3d,ase,ifc,xgl,zgl,lwo,stl,x,ms3d,cob,irrmesh,md1,md2,md3,mdc,md5,ter}",
                                                  current_project.path.c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(!filename)
        return;

    M_importAssimpMeshes(filename);
    update_scene_tree();
}

void delete_object_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->deleteSelectedObjects();

    update_scene_tree();
}

void undo_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->undo();

    update_scene_tree();
}

void redo_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->redo();

    update_scene_tree();
}

void add_sound_callback(Fl_Menu_ *, void *)
{
    if(current_project.level.empty())
    {
        fl_message(tr("You need to save the level before adding objects!"));
        return;
    }

    const char* filename = fl_native_file_chooser(tr("Choose file"), "*.{wav,ogg}", (current_project.path + "sounds").c_str(), Fl_Native_File_Chooser::BROWSE_FILE);

    if(!filename)
        return;

    Maratis::getInstance()->okAddSound(filename);

    update_scene_tree();
}

void edit_sound_callback(Fl_Value_Input*, void*)
{
    OSound* sound = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getSoundByName(window.name_edit->value());

    if(!sound)
        return;

    sound->setPitch(window.sound_pitch_edit->value());
    sound->setGain(window.sound_gain_edit->value());
    sound->setRadius(window.sound_radius_edit->value());
    sound->setRolloff(window.sound_rolloff_edit->value());
}

void edit_sound_chk_btn(Fl_Check_Button*, void*)
{
    OSound* sound = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getSoundByName(window.name_edit->value());

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
    Level* level = NeoEngine::getInstance()->getLevel();
    Scene* scene = level->addNewScene();
    scene->setName(window.scene_name.c_str());

    unsigned int index = level->getScenesNumber()-1;

    change_scene_callback(NULL, index);
    update_scene_tree();
}

void scene_setup_callback(Fl_Menu_ *, void*)
{
    if(current_project.level.empty())
    {
        fl_message(tr("You need to save the level before editing scene properties!"));
        return;
    }

    SceneSetupDlg dlg;
    Fl_Window* win = dlg.create_window();

    Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
    dlg.lua_script_edit->value(scene->getScriptFilename());
    dlg.scene_name_edit->value(scene->getName());

    win->show();
    while(win->shown())
        Fl::wait();

    if(strcmp(win->label(), "Success"))
    {
        // MLOG_INFO("Won't change scene.");
        return;
    }

    Maratis::getInstance()->autoSave();
    scene->setName(dlg.scene_name_edit->value());
    scene->setScriptFilename(dlg.lua_script_edit->value());

    update_scene_tree();
}

void delete_scene_callback(Fl_Menu_ *, void *)
{
    Level* level = NeoEngine::getInstance()->getLevel();

    if(level->getScenesNumber()-1 <= 0)
    {
        MLOG_INFO("Can not delete scene! The level needs at least one scene!");
        fl_alert(tr("Can not delete scene! The level needs at least one scene!"));
        return;
    }

    Maratis::getInstance()->autoSave();
    level->deleteScene(level->getCurrentSceneId());

    Maratis::getInstance()->clearSelectedObjects();
    update_scene_tree();
}

void duplicate_object_callback(Fl_Menu_ *, void *)
{
    Maratis::getInstance()->duplicateSelectedObjects();
    update_scene_tree();
}

void about_menu_callback(Fl_Menu_*, void*)
{
	AboutDlg dlg;
    Fl_Window* window = dlg.create_window();

    window->position(main_window->x()+0.5*main_window->w()-0.5*window->w(), main_window->y()+0.33*main_window->h());
    window->show();
}

void select_all_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->selectAll();
    update_scene_tree();
}

void edit_materials_callback(Fl_Button *, void *)
{
    MaterialEditDlg* dlg = new MaterialEditDlg();
    dlg->create_window(window.name_edit->value())->show();
}

static void copyDirFiles(const char * src, const char * dest, const char * filter)
{
	vector<string> files;
	readDirectory(src, &files);
	for (int i = 0; i < files.size(); ++i)
	{
		if (strstr(files[i].c_str(), filter))
		{
			char filename[256];
			getGlobalFilename(filename, dest, files[i].c_str());
			copyFile(files[i].c_str(), filename);
		}
	}
}

void update_player_callback(Fl_Menu_*, void*)
{
	char src[256];
	char dir[256];

	if(current_project.path.empty())
		return;

#ifndef WIN32
	getGlobalFilename(src, ".", "NeoPlayer");
	getGlobalFilename(dir, current_project.path.c_str(), "NeoPlayer");

    copyDirFiles(".", current_project.path.c_str(), ".so");   
#else
	getGlobalFilename(src, ".", "NeoPlayer.exe");
	getGlobalFilename(dir, current_project.path.c_str(), "NeoPlayer.exe");

	copyDirFiles(".", current_project.path.c_str(), ".dll");
#endif

	copyFile(src, dir);

#ifndef WIN32
    // Make file executable
    chmod(dir, S_IRWXU);
#endif
}

void add_group_callback(Fl_Menu_*, void*)
{
	Maratis::getInstance()->addGroup();

	update_scene_tree();
}

void ortho_callback(Fl_Check_Button*, void*)
{
    Maratis::getInstance()->switchCurrentVueMode();
}

void change_vue_callback(Fl_Menu_*, long mode)
{
	if(mode == 0) return;
	
	Maratis::getInstance()->changeCurrentVue(mode);
	window.vue_ortho_button->value(1);
}

void object_constraint_properties_callback(Fl_Button*, void*)
{
    ConstraintPropertiesDlg* dlg = new ConstraintPropertiesDlg;

    Fl_Window* win = dlg->create_window();
    if(win)
        win->show();
}

void plugin_console_callback(Fl_Menu_*, void*)
{
    if(pluginConsole.closed)
    {
        Fl_Window* window = pluginConsole.create_window();
        window->label(tr("Plugin Console"));
        window->show();
    }

    pluginConsole.output_edit->buffer(&pluginOutput);
}

void configuration_callback(Fl_Menu_*, void*)
{
    ConfigurationDlg dlg;
    Fl_Window* win = dlg.create_window();

    win->show();
    while(win->shown())
        Fl::wait();

    delete win;
}

void post_effects_setup_callback(Fl_Menu_*, void*)
{
    Maratis::getInstance()->autoSave();

    PostEffectsDlg dlg;
    Fl_Window* window = dlg.create_window();

    window->show();
    while(window->shown())
        Fl::wait();

    Fl::delete_widget(window);
}

int redirect_script_print()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
    if(script->getArgsNumber() <= 0)
        return 0;

    const char* text = script->getString(0);
    console_buffer.append(text);
    console_buffer.append("\n");

    if(!console.closed)
    {
        console.output_edit->move_down();
        console.output_edit->show_insert_position();
    }

    return 1;
}

void setCursorPos(int x, int y)
{
#if defined WIN32
    BOOL result = SetCursorPos(x, y);
    if (result) return;
#elif defined __APPLE__
    CGPoint new_pos;
    CGEventErr err;
    new_pos.x = x;
    new_pos.y = y;
    err = CGWarpMouseCursorPosition(new_pos);
    if (!err) return;
#else
    Window rootwindow = DefaultRootWindow(fl_display);
    XWarpPointer(fl_display, rootwindow, rootwindow, 0, 0, 0, 0,::window.glbox->x_root()+x,::window.glbox->y_root()+y);
#endif
}

int centerCursorReplacement()
{
    NeoEngine * engine = NeoEngine::getInstance();
    MSystemContext * system = engine->getSystemContext();
    MInputContext * input = engine->getInputContext();

    unsigned int width = 0;
    unsigned int height = 0;
    system->getScreenSize(&width, &height);
    int x = width/2;
    int y = height/2;

    system->setCursorPosition(x, y);
    setCursorPos(x, y);
    input->setAxis("MOUSE_X", (float)(x / (float)width));
    input->setAxis("MOUSE_Y", (float)(y / (float)height));

    return 0;
}

int quitReplacement()
{
    MLOG_WARNING("Quit is not supported in the editor!");
    return 1;
}

int loadLevelReplacement()
{
    MLOG_WARNING("loadLevel is not supported in the editor!");
    fl_message("loadLevel is not supported in the editor!");
    return 1;
}

void play_game_in_editor(Fl_Button* button, void *)
{
    NeoEngine* engine = NeoEngine::getInstance();
    Level * level = engine->getLevel();
    Scene * scene = level->getCurrentScene();

    NeoGame* game = engine->getGame();

    // Quit the game
    if(game->isRunning())
    {
        game->end();
        return;
    }

    LuaScript scriptContext;
    scriptContext.addFunction("print", redirect_script_print);
    scriptContext.addFunction("centerCursor", centerCursorReplacement);
    scriptContext.addFunction("quit", quitReplacement);
    scriptContext.addFunction("loadLevel", loadLevelReplacement);

	Neo::Gui::GuiSystem::getInstance()->setupLuaInterface(&scriptContext);

    const char* text = button->label();
    button->label(tr("Stop game"));
    console_buffer.text("");

    Fl::focus(::window.glbox);

    // Save perspective vue
    MMatrix4x4 matrix = *Maratis::getInstance()->getPerspectiveVue()->getMatrix();

    engine->setScriptContext(&scriptContext);

    // Save current state
    const char * temp = Maratis::getInstance()->getTempDir();
    if(temp)
    {
        string tempFile(temp);
        tempFile += "/";
        tempFile += "temp.level";

        // save temp level
        xmlLevelSave(engine->getLevel(), tempFile.c_str());
    }

    // play sound
    scene->stopAllSounds();
    scene->playLoopSounds();

    PROFILE_SET_ENABLED(1);
    game->begin();

    // Remains in here because game is a local variable!
    while(engine->getGame()->isRunning())
    {
        PROFILE_UPDATE();
        Fl::wait();
    }

    // Reload old state
    // stop sound
    scene->stopAllSounds();

    // show mouse
    MWindow::getInstance()->showCursor();

    if(temp)
    {
        string tempFile(temp);
        tempFile += "/";
        tempFile += "temp.level";
        M_loadLevel(tempFile.c_str(), level, false);
    }

    scene = level->getCurrentScene();
    Maratis::getInstance()->clearSelectedObjects();

    // update matrices
    scene->updateObjectsMatrices();
    *Maratis::getInstance()->getPerspectiveVue()->getMatrix() = matrix;
    button->label(text);

    update_scene_tree();
	Gui::GuiSystem::getInstance()->clear();

    // Save profile
    PROFILE_OUTPUT((current_project.path + "/profile.txt").c_str());
    PROFILE_SET_ENABLED(0);
}

void show_console_callback(Fl_Button*, void*)
{
    if(console.closed)
    {
        Fl_Window* window = console.create_window();
        window->label(tr("Player Console"));
        window->show();
    }

    console.output_edit->buffer(&console_buffer);
}

void apply_editor_perspective(Fl_Button *, void *)
{
    OCamera* vue = Maratis::getInstance()->getPerspectiveVue();
    Object3d* selectedObject = Maratis::getInstance()->getSelectedObjectByIndex(0);

    if(selectedObject)
    {
        *selectedObject->getMatrix() = *vue->getMatrix();
    }

    Maratis::getInstance()->clearSelectedObjects();
}

void set_editor_perspective(Fl_Button *, void *)
{
    OCamera* vue = Maratis::getInstance()->getPerspectiveVue();
    Object3d* selectedObject = Maratis::getInstance()->getSelectedObjectByIndex(0);

    if(selectedObject)
    {
        vue->setPosition(selectedObject->getTransformedPosition());
        vue->setRotation(selectedObject->getRotation());
    }

    Maratis::getInstance()->clearSelectedObjects();
}

void open_profile_viewer_callback(Fl_Menu_*, void*)
{
    char arg[256];
    getGlobalFilename(arg, current_project.path.c_str(), "profile.txt");

    // TODO: Start detached!
#ifndef WIN32
    MWindow::getInstance()->execute("./ProfileViewer", arg);
#else
	MWindow::getInstance()->execute(".\\ProfileViewer.exe", arg);
#endif
}

// TODO: In own file!
bool check_updates_finished = false;
int check_updates_thread(void* data)
{
	string* str = (string*) data;

	HTTPRequest* connection = new HTTPRequest("neo-engine.de", 4000);

    // FIXME: Don't hardcode this!
    const char* versiontxt = connection->sendGetRequest("/downloads/neo/daily/version.txt");

    check_updates_finished = true;

    if(!versiontxt)
    {
        delete connection;
        return 1;
    }

    str->assign(versiontxt);
    delete connection;
	return 0;
}

void check_for_updates_callback(Fl_Menu_*, void*)
{
	SDLThread thread;
    std::string version;
    check_updates_finished = false;

	thread.Start(check_updates_thread, "CheckUpdates", (void*) &version);

	while(!check_updates_finished)
	{
		Fl::check();
	}

	int ret = thread.WaitForReturn();

	if(ret == 1)
	{
        MLOG_ERROR("Could not retrieve version information from the server!");
        fl_message(tr("Could not retrieve version information from the server!"));
        return;
	}

    int idx = version.find("Version:");
    int vidx = version.find("v");

    if(idx == -1 || vidx == -1)
    {
        MLOG_ERROR("Version data is invalid!");
        fl_message(tr("Version data is invalid! Please try again."));
        return;
    }

    version = version.substr(idx);
    version = version.substr(version.find("v"));
    version.erase(version.length()-1);

    MLOG_INFO("Official version is " << version << ", current version is " << EDITOR_VERSION_STRING);
    if(version != EDITOR_VERSION_STRING)
    {
        fl_message(tr("A new version of this application is available.\nPlease visit our website to download it!"));
    }
    else
    {
        fl_message(tr("You already run the latest and greatest version of this application!"));
    }
}

void local_transform_mode_callback(Fl_Menu_* menu, void*)
{
	Maratis* maratis = Maratis::getInstance();	
	maratis->setOrientationMode(menu->value() ? M_ORIENTATION_LOCAL: M_ORIENTATION_WORLD);
}

void import_lua_sdk_callback(Fl_Menu_ *, void *)
{
	if(current_project.path.empty())
		return;

	char src[255];
	char dir[255];
	getGlobalFilename(src, ".", "LuaApi");
	getGlobalFilename(dir, current_project.path.c_str(), "scripts/SDK");

	copyDirectory(src, dir);
}

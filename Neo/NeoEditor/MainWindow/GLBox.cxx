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

#include "../MainWindow/GLBox.h"

#include <NeoEngine.h>
#include <ImageLoader.h>
#include <GuiSystem.h>

#include "../Maratis/Maratis.h"
#include "../RenderArray/RenderArray.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Double_Window.H>

#include "../PluginScript/PluginScript.h"

#include <algorithm>
#include <string>

#include <Shiny.h>
#include <Window/Keyboard.h>
#include <Window/Mouse.h>
#include <Window/Window.h>
#include "../FilesUpdate/FilesUpdate.h"
#include "../MainWindow/Callbacks.h"
#include "../MainWindow/MainWindow.h"

#include "Utils.h"

using namespace Neo;

extern Fl_Double_Window* main_window;
extern EditorWindow window;

extern void update_scene_tree();

void update_editor(void*)
{
    if(!window.glbox->maratis_init)
        return;

    NeoGame* game = NeoEngine::getInstance()->getGame();
    MInputContext* input = NeoEngine::getInstance()->getInputContext();
    Maratis* maratis = Maratis::getInstance();

    if(window.inputMethod == NULL && !game->isRunning())
    {
        if(input->isKeyPressed("W"))
        {
            int direction = -1;

            OCamera * vue = maratis->getPerspectiveVue();

            if(vue->isOrtho())
            {
                vue->setFov(vue->getFov() + direction*translation_speed);
            }


            vue->setPosition(vue->getPosition() + vue->getRotatedVector(MVector3(0,0,direction*translation_speed)));
            vue->updateMatrix();

            //window.glbox->redraw();
        }
        else if(input->isKeyPressed("S"))
        {
            int direction = 1;

            OCamera * vue = maratis->getPerspectiveVue();
            if(vue->isOrtho())
            {
                vue->setFov(vue->getFov() + direction*translation_speed);
            }

            vue->setPosition(vue->getPosition() + vue->getRotatedVector(MVector3(0,0,direction*translation_speed)));
            vue->updateMatrix();

            //window.glbox->redraw();
        }

        if(input->isKeyPressed("A"))
        {
            int direction = -1;

            OCamera * vue = maratis->getPerspectiveVue();
            vue->setPosition(vue->getPosition() + vue->getRotatedVector(MVector3(direction*translation_speed,0,0)));
            vue->updateMatrix();

            //window.glbox->redraw();
        }
        else if(input->isKeyPressed("D"))
        {
            int direction = 1;

            OCamera * vue = maratis->getPerspectiveVue();
            vue->setPosition(vue->getPosition() + vue->getRotatedVector(MVector3(direction*translation_speed,0,0)));
            vue->updateMatrix();

            //window.glbox->redraw();
        }

        if(input->isKeyPressed("E"))
        {
            OCamera * vue = maratis->getPerspectiveVue();
            vue->setPosition(vue->getPosition()+vue->getRotatedVector(MVector3(0,translation_speed, 0)));
            vue->updateMatrix();

            //window.glbox->redraw();
        }
        else if(input->isKeyPressed("C"))
        {
            OCamera * vue = maratis->getPerspectiveVue();
            vue->setPosition(vue->getPosition()+vue->getRotatedVector(MVector3(0,-translation_speed, 0)));
            vue->updateMatrix();

            //window.glbox->redraw();
        }
    }
    else if(!game->isRunning())
    {
        window.inputMethod->callFunction(window.inputMethod->getInputUpdate().c_str());
    }

    if(maratis->hasTitleChanged())
    {
        main_window->label(maratis->getWindowTitle());
        maratis->setTitleChanged(false);
    }

    if(NeoWindow::getInstance()->getFocus())
    {
        if(game)
        {
            if(!game->isRunning())
                FilesUpdate::update();
            else
                update_scene_tree();
        }
        else
        {
            FilesUpdate::update();
        }
    }

    /*if(!game->isRunning())
    {
        maratis->getPerspectiveVue()->loadSkybox(scene->getCurrentCamera()->getSkybox()->getPath());
    }*/

    NeoWindow::getInstance()->setPosition(window.glbox->x_root() , window.glbox->y_root());
    window.glbox->redraw();

    maratis->logicLoop();
	Neo::Gui::GuiSystem::getInstance()->update();

    if(window.m_deferredUiUpdate)
    {
        window.m_deferredUiUpdate = false;
        create_behavior_ui(maratis->getSelectedObjectByIndex(0));
    }

    Fl::add_timeout(0.015, update_editor);
}

void GLBox::loadPostEffectsFromGame(NeoGame* game)
{
    if(game->hasPostEffects())
    {
        m_postProcessor.loadShaderFile(game->getPostProcessor()->getVertexShader(),
                                                            game->getPostProcessor()->getFragmentShader());
        m_postProcessing = true;
        m_postProcessor.eraseTextures();
        m_postProcessor.updateResolution();
    }
    else
    {
        disablePostEffects();
    }
}

void GLBox::draw()
{
    Maratis* maratis = Maratis::getInstance();
    NeoEngine* engine = NeoEngine::getInstance();
    NeoWindow* window = NeoWindow::getInstance();
    MRenderingContext* render = engine->getRenderingContext();

    if(!maratis_init)
    {
    	maratis->initRenderer();

    	// Get newly created renderer
    	render = engine->getRenderingContext();

        window->setViewport(w(), h());

        if(!current_project.file_path.empty())
        {
            current_project.changed = false;
            current_project.path = current_project.file_path;

    #ifndef WIN32
            current_project.path = current_project.path.erase(current_project.path.find_last_of("/")+1, current_project.path.length());
    #else
            current_project.path = current_project.path.erase(current_project.path.find_last_of("\\")+1, current_project.path.length());
    #endif

            maratis->loadProject(current_project.file_path.c_str());

            ::window.project_directory_browser->load(current_project.path.c_str());
            ::window.glbox->loadPostEffectsFromGame(NeoEngine::getInstance()->getGame());
            current_project.level = maratis->getCurrentLevel();
        }

        update_scene_tree();

        // MLOG_INFO("Render version : " << render->getRendererVersion());

        render->setTextureFilterMode(M_TEX_FILTER_NEAREST, M_TEX_FILTER_NEAREST_MIPMAP_NEAREST);

        // TODO: Don't hardcode this!
        render->setClearColor(MVector4(0.18, 0.32, 0.45, 1));

        m_postProcessor.eraseTextures();
        m_postProcessor.updateResolution();

        maratis_init = true;
        reload_editor = true;
    }

    if(reload_editor)
    {
        reload_editor = false;
    }

    window->setViewport(w(), h());
    render->setScissor(0, 0, w(), h());

    NeoGame* game = engine->getGame();
    if(!game->isRunning())
    {
        engine->updateRequests();

        // TODO: Don't hardcode this!
        render->setClearColor(MVector4(0.18, 0.32, 0.45, 1));

        render->disableScissorTest();
        if(!m_postProcessing || !m_postProcessor.draw(maratis->getPerspectiveVue()))
            maratis->graphicLoop();

        if(maratis->getSelectedObjectsNumber() > 0)
        {
            Object3d* camera = maratis->getSelectedObjectByIndex(0);
            if(camera != NULL && camera->getType() == M_OBJECT3D_CAMERA)
            {
                render->setClearColor(static_cast<OCamera*>(camera)->getClearColor());
                render->setViewport(0,0, w()/3, h()/3);
                render->setScissor(0,0, w()/3, h()/3);
                render->enableScissorTest();

                static_cast<OCamera*>(camera)->enable();

                Scene* scene = engine->getLevel()->getCurrentScene();

                // Render everything
                render->clear(M_BUFFER_DEPTH | M_BUFFER_COLOR);
                scene->draw(static_cast<OCamera*>(camera));
                scene->drawObjectsBehaviors();

                render->setScissor(0,0,w(),h());
                render->disableScissorTest();
            }
        }
    }
    else
    {
        maratis->graphicLoop();
		Neo::Gui::GuiSystem::getInstance()->draw();
    }

    // Workaround for a Bug in Gnome. Should not make a difference though.
#ifndef LINUX
    swap_buffers();
#endif
}

// TODO: Make this a function!
#define ENDS_WITH(s, e) (s.compare(s.length() - strlen(e), strlen(e), e) == 0)

int GLBox::handle(int event)
{
    //fprintf(stderr, "Handle %d FL_KEYBOARD is %d\n", event, FL_KEYBOARD);
    char key[2] = {0,0};
    MInputContext* input = NeoEngine::getInstance()->getInputContext();
    NeoWindow* window = NeoWindow::getInstance();
    Maratis* maratis = Maratis::getInstance();

    switch(event)
    {
    case FL_KEYBOARD:
        {
            if(this != Fl::focus() || (Fl::event_ctrl() && !NeoEngine::getInstance()->getGame()->isRunning()))
                return 0;

            key[0] = toupper(Fl::event_text()[0]);
            NeoEngine::getInstance()->getInputContext()->downKey(key);

            char keys[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"};

            for(int i = 0; i < sizeof(keys); i++)
            {
                char c[2] = {0,0};
                c[0] = keys[i];

                if(Fl::get_key(keys[i]))
                {
                    input->downKey(c);
                }
                else
                {
                    input->upKey(c);
                }
            }

            if(key[0] == ' ')
            {
                input->downKey("SPACE");
                return 1;
            }

            switch(Fl::event_key())
            {
            case FL_Shift_L:
                    input->downKey("LSHIFT");
                break;

            case FL_Shift_R:
                    input->downKey("RSHIFT");
                break;

            case FL_Up:
                    input->downKey("UP");
                    return 1;
                break;

            case FL_Down:
                    input->downKey("DOWN");
                    return 1;
                break;

            case FL_Left:
                    input->downKey("LEFT");
                    return 1;
                break;

            case FL_Right:
                    input->downKey("RIGHT");
                    return 1;
            case FL_Control_L:
                    input->downKey("LCONTROL");
                    return 1;

            case FL_Control_R:
                    input->downKey("RCONTROL");
                    return 1;

            case FL_Alt_L:
                    input->downKey("LALT");
                    return 1;

            case FL_Alt_R:
                    input->downKey("RALT");
                    return 1;

            case FL_Enter:
                    input->downKey("ENTER");
                    return 1;

            case FL_Delete:
                    input->downKey("DELETE");
                    break;

            case FL_Escape:
                    input->downKey("ESCAPE");
                    return 1;

            case FL_Tab:
                    input->downKey("TAB");
                    return 1;

            case FL_BackSpace:
                    input->downKey("BACKSPACE");
                    return 1;

            }

            char fkey[4];
            for(int i = 1; i <= 12; i++)
            {
                if(Fl::event_key() == FL_F + i)
                {
                    sprintf(fkey, "F%d", i);
                    input->downKey(fkey);
                }
            }

            Fl_Gl_Window::handle(event);

            // F6 should _always_ work to end the game!
            if(NeoEngine::getInstance()->getGame()->isRunning() && Fl::event_key() != FL_F + 6)
                return 1;
            else
                return isalpha(Fl::event_text()[0]);
        }
        break;

    case FL_KEYUP:
        {
            if(this != Fl::focus())
                return 0;

            key[0] = toupper(Fl::event_text()[0]);
            char keys[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"};

            for(int i = 0; i < sizeof(keys); i++)
            {
                char c[2] = {0,0};
                c[0] = keys[i];

                if(Fl::get_key(keys[i]))
                {
                    input->downKey(c);
                }
                else
                {
                    input->upKey(c);
                }
            }

            if(key[0] == ' ')
            {
                input->upKey("SPACE");
                return 1;
            }

            switch(Fl::event_key())
            {
            case FL_Shift_L:
                    input->upKey("LSHIFT");
                break;

            case FL_Shift_R:
                    input->upKey("RSHIFT");
                break;

            case FL_Up:
                    input->upKey("UP");
                    return 1;
                break;

            case FL_Down:
                    input->upKey("DOWN");
                    return 1;
                break;

            case FL_Left:
                    input->upKey("LEFT");
                    return 1;
                break;

            case FL_Right:
                    input->upKey("RIGHT");
                    return 1;
            case FL_Control_L:
                    input->upKey("LCONTROL");
                    return 1;

            case FL_Control_R:
                    input->upKey("RCONTROL");
                    return 1;

            case FL_Alt_L:
                    input->upKey("LALT");
                    return 1;

            case FL_Alt_R:
                    input->upKey("RALT");
                    return 1;

            case FL_Enter:
                    input->upKey("ENTER");
                    return 1;

            case FL_Delete:
                    input->upKey("DELETE");
                    break;

            case FL_Escape:
                    input->upKey("ESCAPE");
                    return 1;

            case FL_Tab:
                    input->upKey("TAB");
                    return 1;

            case FL_BackSpace:
                    input->upKey("BACKSPACE");
                    return 1;
            }

            char fkey[4];
            for(int i = 1; i <= 12; i++)
            {
                if(Fl::event_key() == FL_F + i)
                {
                    sprintf(fkey, "F%d", i);
                    input->upKey(fkey);
                }
            }

            Fl_Gl_Window::handle(event);
            return 1;
        }
        break;

    case FL_MOVE:
        {
            MMouse* mouse = MMouse::getInstance();
            mouse->setDirection(mouse_x-Fl::event_x(), mouse_y-Fl::event_y());

            mouse_x = Fl::event_x();
            mouse_y = Fl::event_y();

            mouse->setPosition(mouse_x, mouse_y);
            maratis->updateCurrentAxis();

        break;
        }
    case FL_MOUSEWHEEL:
        {
            int direction = Fl::event_dy();

            MMouse* mouse = MMouse::getInstance();
            mouse->setWheelDirection(direction);

            MInputContext* input = NeoEngine::getInstance()->getInputContext();
            input->setAxis("MOUSE_WHEEL", input->getAxis("MOUSE_WHEEL") + direction);

            if(::window.inputMethod == NULL)
            {
                OCamera * vue = maratis->getPerspectiveVue();
                vue->setPosition(vue->getPosition() + vue->getRotatedVector(MVector3(0,0,direction*translation_speed)));
                vue->updateMatrix();
            }

            redraw();
            return 1;
        }
        break;

    case FL_FOCUS:
            //Fl::focus(this);
            NeoWindow::getInstance()->setFocus(true);
            return 1;
        break;

    case FL_UNFOCUS:
            NeoWindow::getInstance()->setFocus(false);
            return 1;
        break;

    case FL_PUSH:

            Fl::focus(this);

            mouse_x = Fl::event_x();
            mouse_y = Fl::event_y();

            MMouse::getInstance()->setPosition(mouse_x, mouse_y);

            if(Fl::event_button1() && !NeoEngine::getInstance()->getGame()->isRunning())
            {
                maratis->selectObjectsInMainView(NeoEngine::getInstance()->getLevel()->getCurrentScene(), Fl::event_shift() > 0);

                maratis->lockSelection();
                ::window.scene_tree->deselect_all();

                Fl_Tree_Item* item = ::window.scene_tree->root();
                if(maratis->getSelectedObjectsNumber() > 0)
                {
                	while((item = ::window.scene_tree->next(item)) != NULL)
					{
                		for(int i = 0; i < maratis->getSelectedObjectsNumber(); i++)
                		{
							if(item && !strcmp(item->label(), maratis->getSelectedObjectByIndex(i)->getName()))
							{
								::window.scene_tree->select(item);
							}
                		}
					}
                }
                maratis->unlockSelection();

                update_name = true;
                redraw();
                ::window.special_tab->redraw();
            }

            {
                    MInputContext* input = NeoEngine::getInstance()->getInputContext();

                    switch(Fl::event_button())
                    {

                        case FL_MIDDLE_MOUSE:
                        {
                            MMouse::getInstance()->downButton(MMOUSE_BUTTON_MIDDLE);
                            input->onKeyDown("MOUSE_BUTTON_MIDDLE");
                            input->downKey("MOUSE_BUTTON_MIDDLE");
                        }
                        break;

                        case FL_RIGHT_MOUSE:
                        {
                            MMouse::getInstance()->downButton(MMOUSE_BUTTON_RIGHT);
                            input->onKeyDown("MOUSE_BUTTON_RIGHT");
                            input->downKey("MOUSE_BUTTON_RIGHT");
                        }

                        case FL_LEFT_MOUSE:
                        {
                            MMouse::getInstance()->downButton(MMOUSE_BUTTON_LEFT);
                            input->onKeyDown("MOUSE_BUTTON_LEFT");
                            input->downKey("MOUSE_BUTTON_LEFT");
                        }
                    }
            }
            return 1;
        break;

    case FL_RELEASE:
    {
        MInputContext* input = NeoEngine::getInstance()->getInputContext();

        switch(Fl::event_button())
        {

            case FL_MIDDLE_MOUSE:
            {
                MMouse::getInstance()->upButton(MMOUSE_BUTTON_MIDDLE);
                input->onKeyUp("MOUSE_BUTTON_MIDDLE");
                input->upKey("MOUSE_BUTTON_MIDDLE");
            }
            break;

            case FL_RIGHT_MOUSE:
            {
                MMouse::getInstance()->upButton(MMOUSE_BUTTON_RIGHT);
                input->onKeyUp("MOUSE_BUTTON_RIGHT");
                input->upKey("MOUSE_BUTTON_RIGHT");
            }

            case FL_LEFT_MOUSE:
            {
                MMouse::getInstance()->upButton(MMOUSE_BUTTON_LEFT);
                input->onKeyUp("MOUSE_BUTTON_LEFT");
                input->upKey("MOUSE_BUTTON_LEFT");
            }
        }
        return 1;
    }
        break;

    case FL_DRAG:
        {
            NeoGame* game = NeoEngine::getInstance()->getGame();
            if(Fl::event_button3() && ::window.inputMethod == NULL && !game->isRunning())
            {
                OCamera * vue = maratis->getPerspectiveVue();

                vue->setEulerRotation(vue->getEulerRotation() + MVector3(mouse_y - Fl::event_y(), 0, mouse_x-Fl::event_x())*0.5*rotation_speed);
                vue->updateMatrix();
            }

            MMouse* mouse = MMouse::getInstance();
            mouse->setDirection(mouse_x-Fl::event_x(), mouse_y-Fl::event_y());

            mouse_x = Fl::event_x();
            mouse_y = Fl::event_y();

            mouse->setPosition(mouse_x, mouse_y);
            if(Fl::event_button1() && !game->isRunning())
            {
                maratis->transformSelectedObjects();
            }

            redraw();
        }
        break;

    case FL_DND_ENTER:
    case FL_DND_DRAG:
    case FL_DND_RELEASE:
        return 1;
        break;

    case FL_PASTE:
            {
                std::string filename = Fl::event_text();

                // Some WMs like to add a '\n' to the path.
                if(ENDS_WITH(filename, "\n"))
                    filename.erase(filename.end()-1);

                std::string suffix = filename;

                // Window does not add 'file://' to the path.
				#ifndef WIN32
                if(filename.find("file://") != 0)
                    return 1;

				filename = filename.substr(7);
				#endif
  
                // Don't try to load anything if path is shorter than the shortest file extension
                if(filename.length() < 4)
                    return 1;

                // Convert path to lowercase
                std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

                if(ENDS_WITH(suffix, ".mproj") != 0)
                {
                    if(!current_project.path.empty() && !ask("You need to close the current project. Do you want to proceed?"))
                        return 1;

                    current_project.path = filename;
                    current_project.file_path = filename;

                #ifndef WIN32
                    current_project.path = current_project.path.erase(current_project.path.find_last_of("/")+1, current_project.path.length());
                #else
                    current_project.path = current_project.path.erase(current_project.path.find_last_of("\\")+1, current_project.path.length());
                #endif

                    maratis->loadProject(filename.c_str());

                    NeoGame* game = NeoEngine::getInstance()->getGame();

                    ::window.project_directory_browser->load(current_project.path.c_str());
                    loadPostEffectsFromGame(NeoEngine::getInstance()->getGame());

                    current_project.level = maratis->getCurrentLevel();
                    reload_editor = true;
                }
                else if(ENDS_WITH(suffix, ".level"))
                {
                    maratis->loadLevel(filename.c_str());
                    current_project.level = filename;
                }
                else if(ENDS_WITH(suffix, ".mesh"))
                {
                    maratis->okAddEntity(filename.c_str());
                }
                else if(ENDS_WITH(suffix, ".ttf"))
                {
                    maratis->okAddFont(filename.c_str());
                }
                else if(ENDS_WITH(suffix, ".wav") || ENDS_WITH(suffix, ".ogg") || ENDS_WITH(suffix, ".aiff"))
                {
                    maratis->okAddSound(filename.c_str());
                }
                else
                {
                    MLOG_ERROR("Could not load file on drag'n'drop: " << filename);
                    fl_message("Could not load the file you dropped. File type is unknown!");
                }

                update_scene_tree();
                return 1;
            }
        break;
    }

    if(input)
    {
        input->setAxis("MOUSE_X", (static_cast<float>(mouse_x)/w()));
        input->setAxis("MOUSE_Y", (static_cast<float>(mouse_y)/h()));
    }

    Fl_Tree_Item* item = ::window.scene_tree->first_selected_item();

    if(item)
    {
        update_name = false;
        ::window.scene_tree->deselect(item);
        ::window.scene_tree->select(item);
        ::window.special_tab->redraw();
        update_name = true;
    }

    return Fl_Gl_Window::handle(event);
}

void GLBox::resize(int x, int y, int w, int h)
{
    Fl_Gl_Window::resize(x,y,w,h);

    if(m_postProcessing)
    {
        NeoWindow::getInstance()->setViewport(w, h);
        m_postProcessor.eraseTextures();
        m_postProcessor.updateResolution();
    }
}

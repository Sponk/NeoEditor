/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Neo
// FltkToLua.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <www.scary-squid.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "MPluginScript.h"
#include <FL/Fl.H>
#include <FL/fl_message.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <string>
#include <vector>

#include "../FLUID/MainWindow.h"
#include "../Maratis/Maratis.h"

// TODO: All globals in own file!
#include "../FLUID/Callbacks.h"

// Declared in Callbacks.h and implemented in Callbacks.cpp
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);
extern EditorWindow window;

typedef struct
{
    MPluginScript* plugin;
    std::string function;

} lua_callback_t;

std::vector<lua_callback_t> callbacks;

// LuaCallback
void lua_callback(Fl_Widget*, long id)
{
    lua_callback_t callback = callbacks[id];
    callback.plugin->callFunction(callback.function.c_str());
}

int messagebox()
{
    MScript* script = (MScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    fl_message("%s", script->getString(0));
    return 1;
}

// Implemented in Callbacks.cpp. FIXME: Encapsulation!
extern const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);

int fileChooser(int mode)
{
    MScript* script = (MScript*) MEngine::getInstance()->getScriptContext();

    const char* dir = ".";
    const char* files = "*.*";

    if(script->getArgsNumber() >= 2)
        dir = script->getString(1);

    if(script->getArgsNumber() >= 3)
        files = script->getString(2);

    const char* title = script->getString(0);
    script->pushString(fl_native_file_chooser(title, files, dir, mode));

    return 1;
}

// openFileDlg(title, dir, files)
int openFileDlg()
{
    return fileChooser(Fl_Native_File_Chooser::BROWSE_FILE);
}

// openFileDlg(title, dir, files)
int saveFileDlg()
{
    return fileChooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
}

// addEditorMenu(name, callbackName)
int addEditorMenu()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 2)
        return 0;

    const char* path = script->getString(0);
    const char* function = script->getString(1);

    std::string completePath = "Plugins/" + script->getName() + "/" + path;

    lua_callback_t callback;
    callback.function = function;
    callback.plugin = script;

    callbacks.push_back(callback);

    window.menu_bar->add(completePath.c_str(), 0, (Fl_Callback*) lua_callback, (void*) (callbacks.size()-1));
    window.menu_bar->redraw();

    return 1;
}

int getCurrentSelection()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();
    Maratis* maratis = Maratis::getInstance();

    // FIXME: Pointers have 64bit size on 64bit machines! DON'T USE FLOAT
    int* objects = new int[maratis->getSelectedObjectsNumber()];

    for(int i = 0; i < maratis->getSelectedObjectsNumber(); i++)
    {
        MObject3d* object = maratis->getSelectedObjectByIndex(i);
        objects[i] = (long int) object;
    }

    script->pushIntArray(objects, maratis->getSelectedObjectsNumber());
    delete[] objects;

    return 1;
}

int getSelectionCenter()
{
    MScript* script = (MScript*) MEngine::getInstance()->getScriptContext();
    script->pushFloatArray(*Maratis::getInstance()->getSelectionCenter(), 3);

    return 1;
}

int getProjectDir()
{
    MEngine::getInstance()->getScriptContext()->pushString(current_project.path.c_str());
    return 1;
}

int updateEditorView()
{
    // FIXME: Somehow needed or else scripts will crash
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();
    script->pushInteger(0);

    update_scene_tree();
    window.glbox->redraw();
    return 1;
}

int Fl_CreateWindow()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 5)
		return 0;

	int x, y, w, h;
	const char* title;

	x = script->getInteger(0);
	y = script->getInteger(1);
	w = script->getInteger(2);
	h = script->getInteger(3);
	title = script->getString(4);

	Fl_Window* window = new Fl_Window(x, y, w, h, title);
	script->pushPointer(window);
	
	return 1;
}

int Fl_CreateButton()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 5)
		return 0;

	int x, y, w, h;
	const char* title;

	x = script->getInteger(0);
	y = script->getInteger(1);
	w = script->getInteger(2);
	h = script->getInteger(3);
	title = script->getString(4);

	Fl_Button* button = new Fl_Button(x, y, w, h, title);
	script->pushPointer(button);

	return 1;
}

int Fl_CreateInput()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 5)
		return 0;

	int x, y, w, h;
	const char* title;

	x = script->getInteger(0);
	y = script->getInteger(1);
	w = script->getInteger(2);
	h = script->getInteger(3);
	title = script->getString(4);

	Fl_Input* input = new Fl_Input(x, y, w, h, title);
	script->pushPointer(input);

	return 1;
}

int Fl_Add()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 2)
		return 0;

	// Test if argument is a window
	Fl_Window* parent = dynamic_cast<Fl_Window*>(static_cast<Fl_Widget*>(script->getPointer(0)));
	Fl_Widget* child = (Fl_Widget*)script->getPointer(1);

	if (parent == NULL)
	{
		MLOG_ERROR("Object is not of type Fl_Window!");
		return 0;
	}

	parent->add(child);
	return 1;
}

// Fl_Show(widget, shown)
int Fl_Show()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() < 1)
		return 0;

	bool show = true;
	if(script->getArgsNumber() >= 2)
		show = script->getInteger(1) != 0;

	Fl_Widget* widget = static_cast<Fl_Widget*>(script->getPointer(0));
	
	if(show)
		widget->show();
	else
		widget->hide();

	return 1;
}

int Fl_SetCallback()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 2)
		return 0;

	Fl_Widget* widget = (Fl_Widget*) script->getPointer(0);

	lua_callback_t callback;
	callback.function = script->getString(1);
	callback.plugin = script;

	callbacks.push_back(callback);

	widget->callback((Fl_Callback*)lua_callback, (void*)(callbacks.size() - 1));
	return 1;
}

int Fl_DeleteWidget()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 1)
		return 0;

	Fl::delete_widget(static_cast<Fl_Widget*>(script->getPointer(0)));
	return 1;
}

int Fl_InputValue()
{
	MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() < 1)
		return 0;

	Fl_Input* widget = static_cast<Fl_Input*>(script->getPointer(0));

	if (script->getArgsNumber() >= 2)
	{
		widget->value(script->getString(1));
	}
	else
	{
		script->pushString(widget->value());
	}
	return 1;
}

int getPerspectiveVue()
{
    MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

    script->pushInteger((long int) Maratis::getInstance()->getPerspectiveVue());
    return 1;
}

int getTranslationSpeed()
{
    MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

    script->pushFloat(translation_speed);
    return 1;
}

int getRotationSpeed()
{
    MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

    script->pushFloat(rotation_speed);
    return 1;
}

// FIXME: Global variable!
extern Fl_Text_Buffer pluginOutput;

int debugLog()
{
    MPluginScript* script = (MPluginScript*)MEngine::getInstance()->getScriptContext();

    if (script->getArgsNumber() < 1)
        return 0;

    string output = script->getName() + ": " + script->getString(0);
    pluginOutput.append(("[ Info ] " + output).c_str());
    MLOG_INFO(output.c_str());

    return 1;
}

void createFltkLuaBindings(MScript* script)
{
    if(!script)
        return;

    script->addFunction("debugLog", debugLog);
    script->addFunction("messagebox", messagebox);
    script->addFunction("addEditorMenu", addEditorMenu);
    script->addFunction("openFileDlg", openFileDlg);
    script->addFunction("saveFileDlg", saveFileDlg);
    script->addFunction("getCurrentSelection", getCurrentSelection);
    script->addFunction("getProjectDir", getProjectDir);
    script->addFunction("getSelectionCenter", getSelectionCenter);
    script->addFunction("updateEditorView", updateEditorView);
    script->addFunction("getPerspectiveVue", getPerspectiveVue);
    script->addFunction("getTranslationSpeed", getTranslationSpeed);
    script->addFunction("getRotationSpeed", getRotationSpeed);

	script->addFunction("Fl_CreateWindow", Fl_CreateWindow);
	script->addFunction("Fl_Show", Fl_Show);
	script->addFunction("Fl_SetCallback", Fl_SetCallback);
	script->addFunction("Fl_CreateButton", Fl_CreateButton);
	script->addFunction("Fl_Add", Fl_Add);
	script->addFunction("Fl_DeleteWidget", Fl_DeleteWidget);
	script->addFunction("Fl_CreateInput", Fl_CreateInput);
	
	// Needs own function for every widget type
	script->addFunction("Fl_InputValue", Fl_InputValue);
}

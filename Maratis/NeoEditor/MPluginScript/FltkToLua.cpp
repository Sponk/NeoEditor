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

    if(script->getArgsNumber() == 2)
        dir = script->getString(1);
    else if(script->getArgsNumber() == 3)
        files = script->getString(2);

    const char* title = script->getString(0);

    script->pushString(fl_native_file_chooser(title, files, dir, Fl_Native_File_Chooser::BROWSE_FILE));

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

void createFltkLuaBindings(MScript* script)
{
    if(!script)
        return;

    script->addFunction("messagebox", messagebox);
    script->addFunction("addEditorMenu", addEditorMenu);
    script->addFunction("openFileDlg", openFileDlg);
    script->addFunction("saveFileDlg", saveFileDlg);
    script->addFunction("getCurrentSelection", getCurrentSelection);
}

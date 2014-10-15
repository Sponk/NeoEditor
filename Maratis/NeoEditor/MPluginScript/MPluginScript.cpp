/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Neo
// MPluginScript.cpp
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

// Implemented in FltkToLua.cpp
void createFltkLuaBindings(MScript* script);

// Methods to setup plugin data (name, author, license etc.)
int MPluginScript::setName()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_name = script->getString(0);
    return 1;
}

int MPluginScript::setAuthor()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_author = script->getString(0);
    return 1;
}

int MPluginScript::setContact()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_contact = script->getString(0);
    return 1;
}

int MPluginScript::setLicense()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_license = script->getString(0);
    return 1;
}

int MPluginScript::setDescription()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_description = script->getString(0);
    return 1;
}

int MPluginScript::enableInputMethod()
{
    MPluginScript* script = (MPluginScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_hasInputMethod = true;
    script->m_inputUpdate = script->getString(0);
    return 1;
}

// Overwrite some methods from MScript to update the script context
void MPluginScript::init()
{
    m_hasInputMethod = false;

    addFunction("setPluginName", MPluginScript::setName);
    addFunction("setPluginAuthor", MPluginScript::setAuthor);
    addFunction("setPluginLicense", MPluginScript::setLicense);
    addFunction("setPluginDescription", MPluginScript::setDescription);
    addFunction("enableInputMethod", MPluginScript::enableInputMethod);

    // Add FLTK bindings
    createFltkLuaBindings(this);

    MScript::init();
}

void MPluginScript::runScript(const char* filename)
{
    MEngine::getInstance()->setScriptContext(this);
    char g_currentDirectory[256];

    clear();

    if(! filename)
    {
        m_isRunning = false;
        return;
    }

    if(strlen(filename) == 0)
    {
        m_isRunning = false;
        return;
    }

    // current directory
    getRepertory(g_currentDirectory, filename);

    // read file
    char * text = readTextFile(filename);
    if(! text)
    {
        MLOG_ERROR("Script: Unable to read file " << filename);
        m_isRunning = false;
        return;
    }

    init();

    // do string
    if(luaL_dostring(m_state, text) != 0)
    {
        MLOG_ERROR("Lua Script: \n" << lua_tostring(m_state, -1) << "\n");
        m_isRunning = false;
        SAFE_FREE(text);
        return;
    }

    // finish
    SAFE_FREE(text);
    m_isRunning = true;

    MLOG_INFO("Loaded plugin: \n\tName: " << m_name << "\n\tAuthor: " << m_author << "\n\tDescription: " << m_description << "\n\tLicense: " << m_license);
}

void MPluginScript::callFunction(const char* name)
{
    MEngine* engine = MEngine::getInstance();
    MScriptContext* oldContext = engine->getScriptContext();

    engine->setScriptContext(this);
    MScript::callFunction(name);
    engine->setScriptContext(oldContext);
}

bool MPluginScript::startCallFunction(const char *name)
{
    MEngine::getInstance()->setScriptContext(this);
    return MScript::startCallFunction(name);
}

bool MPluginScript::endCallFunction(int numArgs)
{
    MEngine::getInstance()->setScriptContext(this);
    return MScript::endCallFunction(numArgs);
}

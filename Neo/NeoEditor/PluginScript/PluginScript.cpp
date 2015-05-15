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

#include "../PluginScript/PluginScript.h"
#include <Neo2DEngine.h>

using namespace Neo;

// Implemented in FltkToLua.cpp
void createFltkLuaBindings(LuaScript* script);

// Methods to setup plugin data (name, author, license etc.)
int PluginScript::setName()
{
    PluginScript* script = (PluginScript*) NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_name = script->getString(0);
    return 1;
}

int PluginScript::setAuthor()
{
    PluginScript* script = (PluginScript*) NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_author = script->getString(0);
    return 1;
}

int PluginScript::setContact()
{
    PluginScript* script = (PluginScript*) NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_contact = script->getString(0);
    return 1;
}

int PluginScript::setLicense()
{
    PluginScript* script = (PluginScript*) NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_license = script->getString(0);
    return 1;
}

int PluginScript::setDescription()
{
    PluginScript* script = (PluginScript*) NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_description = script->getString(0);
    return 1;
}

int PluginScript::enableInputMethod()
{
    PluginScript* script = (PluginScript*) NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    script->m_hasInputMethod = true;
    script->m_inputUpdate = script->getString(0);
    return 1;
}

// Overwrite some methods from MScript to update the script context
void PluginScript::init()
{
    m_hasInputMethod = false;

    addFunction("setPluginName", PluginScript::setName);
    addFunction("setPluginAuthor", PluginScript::setAuthor);
    addFunction("setPluginLicense", PluginScript::setLicense);
    addFunction("setPluginDescription", PluginScript::setDescription);
    addFunction("enableInputMethod", PluginScript::enableInputMethod);

    // Add FLTK bindings
    createFltkLuaBindings(this);

    LuaScript::init();

    // Add NeoGui bindings
    Neo2D::Neo2DEngine::getInstance()->setupLuaInterface(this);
}

bool PluginScript::runScript(const char* filename)
{
    NeoEngine::getInstance()->setScriptContext(this);
    char g_currentDirectory[256];

    clear();

    if(! filename)
    {
        m_isRunning = false;
        return false;
    }

    if(strlen(filename) == 0)
    {
        m_isRunning = false;
        return false;
    }

    // current directory
    getRepertory(g_currentDirectory, filename);

    // read file
    char * text = readTextFile(filename);
    if(! text)
    {
        MLOG_ERROR("Script: Unable to read file " << filename);
        m_isRunning = false;
        return false;
    }

    init();

    // do string
    if(luaL_dostring(m_state, text) != 0)
    {
        MLOG_ERROR("Lua Script: \n" << lua_tostring(m_state, -1) << "\n");
        m_isRunning = false;
        SAFE_FREE(text);
        return false;
    }

    // finish
    SAFE_FREE(text);
    m_isRunning = true;

    MLOG_INFO("Loaded plugin: \n\tName: " << m_name << "\n\tAuthor: " << m_author << "\n\tDescription: " << m_description << "\n\tLicense: " << m_license);
    return true;
}

void PluginScript::callFunction(const char* name)
{
    NeoEngine* engine = NeoEngine::getInstance();
    ScriptContext* oldContext = engine->getScriptContext();

    engine->setScriptContext(this);
    LuaScript::callFunction(name);
    engine->setScriptContext(oldContext);
}

bool PluginScript::startCallFunction(const char *name)
{
    NeoEngine::getInstance()->setScriptContext(this);
    return LuaScript::startCallFunction(name);
}

bool PluginScript::endCallFunction(int numArgs)
{
    NeoEngine::getInstance()->setScriptContext(this);
    return LuaScript::endCallFunction(numArgs);
}

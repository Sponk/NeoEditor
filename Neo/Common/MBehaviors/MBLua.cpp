/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBLua.cpp
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


#include <MEngine.h>
#include <MScript/MScript.h>
#include <MLog.h>
#include <lua.hpp>
#include <map>
#include "MBLua.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MBLua::MBLua(MObject3d * parentObject):
MBehavior(parentObject),
m_scriptFile(""),
m_init(false)
{}

MBLua::MBLua(MBLua & behavior, MObject3d * parentObject):
MBehavior(parentObject),
m_scriptFile(behavior.m_scriptFile),
m_init(false)
{}

MBLua::~MBLua(void)
{}

void MBLua::destroy(void)
{
	delete this;
}

MBehavior * MBLua::getNew(MObject3d * parentObject)
{
	return new MBLua(parentObject);
}

MBehavior * MBLua::getCopy(MObject3d * parentObject)
{
	return new MBLua(*this, parentObject);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int MBLua::getVariablesNumber(void)
{
    MEngine* engine = MEngine::getInstance();
    // TODO: New variable type for paths!
    std::vector<std::string> names;
    if(!m_init)
    {
        std::vector<MValueVariable*> oldVars;

        for(int i = 0; i < m_globalVariables.size(); i++)
        {
            oldVars.push_back(m_globalVariables[i]);
        }

        m_globalVariables.clear();
        m_variables.clear();

        char globalFile[256];
        getGlobalFilename(globalFile, engine->getSystemContext()->getWorkingDirectory(), m_scriptFile.getSafeString());

        // If the script does not exist, show only the static variable
		if(!isFileExist(globalFile) || isDirectory(globalFile))
        {
            //MLOG_ERROR("Script file does not exist (" << globalFile << ")!");
            return 1;
        }

        m_script.runScript(globalFile);

        lua_State* L = m_script.getLuaState();

        if(L)
        {
            lua_getglobal(L, "public");

            if(lua_isnil(L, -1))
            {
                MLOG_INFO("No global variables in script!");
            }
            else
            {
                lua_pushnil(L);
                while (lua_next(L,-2) != 0)
                {
                    std::string name = lua_tostring(L,-2);
                    names.push_back(name);
                    lua_pop(L,1);
                }
                lua_pop(L,1); // global table


                int n = lua_gettop(L);
                lua_pop(L, n);

                for(int i = 0; i < names.size(); i++)
                {
                    lua_getglobal(L, "public");
                    lua_pushstring(L, names[i].c_str());
                    lua_gettable(L, -2);

                    if(lua_isnil(L, -1))
                    {
                        MLOG_INFO("Variable is nil!");
                    }
                    else if(lua_isnumber(L, -1))
                    {
                        MFloatVariable* var = NULL;
                        for(int j = 0; j < oldVars.size(); j++)
                        {
                            // MLOG_INFO("----> " << floatTmp[j]->var->getName());
                            if(names[i] == oldVars[j]->var->getName())
                            {
                                var = (MFloatVariable*) oldVars[j];
                                oldVars.erase(oldVars.begin() + j);
                            }
                        }

                        if(var == NULL)
                        {
                            var = new MFloatVariable(names[i].c_str());
                            var->value = lua_tonumber(L, -1);
                        }

                        m_globalVariables.push_back(var);
                    }
                    else if(lua_isstring(L, -1))
                    {
                        MStringVariable* var = NULL;
                        for(int j = 0; j < oldVars.size(); j++)
                        {
                            // MLOG_INFO("----> " << floatTmp[j]->var->getName());
                            if(names[i] == oldVars[j]->var->getName())
                            {
                                var = (MStringVariable*) oldVars[j];
                                oldVars.erase(oldVars.begin() + j);
                            }
                        }

                        if(var == NULL)
                        {
                            var = new MStringVariable(names[i].c_str());
                            var->value.set(lua_tostring(L, -1));
                        }

                        m_globalVariables.push_back(var);
                    }
                }
            }
        }

        // MLOG_INFO("floatTmp.size == " << floatTmp.size());
        for(int i = 0; i < oldVars.size(); i++)
        {
            SAFE_DELETE(oldVars[i]);
        }

        for(int i = 0; i < m_globalVariables.size(); i++)
        {
            m_variables.push_back(m_globalVariables[i]->var);
        }
    }

    return m_variables.size() + 1;
}

MVariable MBLua::getVariable(unsigned int id)
{
	switch(id)
	{
	case 0:
		return MVariable("scriptFile", &m_scriptFile, M_VARIABLE_STRING);
    default:
        if(id - 1 > m_variables.size())
            return MVariable("NULL", NULL, M_VARIABLE_NULL);

        return *m_variables[id-1];
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void MBLua::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MGame * game = engine->getGame();
	MObject3d * parent = getParentObject();

    if(! game->isRunning())
            return;

    if(!m_init)
    {
        char globalFile[256];
        getGlobalFilename(globalFile, engine->getSystemContext()->getWorkingDirectory(), m_scriptFile.getSafeString());
        m_script.runScript(globalFile);
        m_init = true;
    }

    lua_State* L = m_script.getLuaState();

    lua_getglobal(L, "public");
    int top = lua_gettop(L);

    for(int i = 0; i < m_variables.size(); i++)
    {
        lua_pushstring(L, m_variables[i]->getName());

        switch(m_variables[i]->getType())
        {
        case M_VARIABLE_FLOAT:
                lua_pushnumber(L, *(float*) m_variables[i]->getPointer());
            break;
        }

        lua_settable(L, top);
    }

    if(m_script.startCallFunction("onSceneUpdate"))
    {
        m_script.pushPointer(parent);
        m_script.endCallFunction(1);
    }
}

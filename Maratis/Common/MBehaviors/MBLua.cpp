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
    return 1;
}

// TODO: Make variable available in the editor!
MVariable MBLua::getVariable(unsigned int id)
{
	// TODO: New variable type for paths!
	switch(id)
	{
	default:
		return MVariable("NULL", NULL, M_VARIABLE_NULL);
	case 0:
		return MVariable("scriptFile", &m_scriptFile, M_VARIABLE_STRING);
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

    if(m_script.startCallFunction("onSceneUpdate"))
    {
        m_script.pushPointer(parent);
        m_script.endCallFunction(1);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBLua.h
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


#ifndef _MB_LUA_H
#define _MB_LUA_H

#include <MScript/MScript.h>
#include <MVariable.h>
#include <vector>
#include <string>
#include <MLog.h>

class MBLua : public MBehavior
{
public:

	// constructors / destructors
	MBLua(MObject3d * parentObject);
	MBLua(MBLua & behavior, MObject3d * parentObject);
	~MBLua(void);

private:

	// variables
	MString m_scriptFile;
    MScript m_script;
    bool m_init;

    struct MValueVariable
    {
        char name[256];
        MVariable* var;
    };

    struct MFloatVariable : public MValueVariable
    {
        MFloatVariable(const char* name)
        {
            strcpy(this->name, name);
            var = new MVariable(this->name, &value, M_VARIABLE_FLOAT);
        }

        ~MFloatVariable()
        {
            SAFE_DELETE(var);
        }

        float value;
    };

    struct MStringVariable : public MValueVariable
    {
        MStringVariable(const char* name)
        {
            strcpy(this->name, name);
            var = new MVariable(this->name, &value, M_VARIABLE_STRING);
        }

        ~MStringVariable()
        {
            SAFE_DELETE(var);
        }

        MString value;
    };

    std::vector<MValueVariable*> m_globalVariables;
    std::vector<MVariable*> m_variables;

public:

	// destroy
	void destroy(void);

	// get new
	static MBehavior * getNew(MObject3d * parentObject);

	// get copy
	MBehavior * getCopy(MObject3d * parentObject);

	// name
	static const char * getStaticName(void){ return "LuaScript"; }
	const char * getName(void){ return getStaticName(); }

	// events
	void update(void);
	void runEvent(int param){}

	// variables
	unsigned int getVariablesNumber(void);
	MVariable getVariable(unsigned int id);
};

#endif

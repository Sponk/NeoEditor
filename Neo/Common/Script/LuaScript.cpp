//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#include <LuaScript.h>
#include <ScriptApi.h>
#include <NeoEngine.h>
#include <Window/Window.h>

using namespace Neo;

extern char g_currentDirectory[256];
extern unsigned long g_startTick;
const char * LUA_VEC3 = "LUA_VEC3";

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some frequently used macros
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_OBJECT_SUBCLASS_BEGIN(type_, var_, type_enum)	\
	MObject3d * object;	\
	lua_Integer id = lua_tointeger(L, 1);	\
	if((object = getObject3d(id)))	\
	{	\
		if(object->getType() == type_enum)	\
		{	\
			type_ * var_ = (type_*)object;

#define GET_OBJECT_SUBCLASS_END()	\
		}	\
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// useful functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool isFunctionOk(lua_State * L, const char * name, unsigned int nbArgs)
{
	int nbArguments = lua_gettop(L);
	if(nbArguments < (int)nbArgs)
	{
        MLOG_ERROR("Script: \"" << name << "\" needs at least " << nbArgs << " parameter(s)!");
		return false;
	}
	return true;
}

static void pushFloatArray(lua_State * L, float * values, unsigned int nbValues)
{
	lua_newtable(L);
	
	for(unsigned int i=0; i<nbValues; i++)
	{
		lua_pushinteger(L, i+1);
		lua_pushnumber(L, values[i]);
		lua_rawset(L, -3);
	}
	
	if(nbValues == 3) // vec3
	{
		luaL_getmetatable(L, LUA_VEC3);
		lua_setmetatable(L, -2);
	}
}

static void pushIntArray(lua_State * L, lua_Integer * values, unsigned int nbValues)
{
	lua_newtable(L);
	
	for(unsigned int i=0; i<nbValues; i++)
	{
		lua_pushinteger(L, i+1);
		lua_pushinteger(L, values[i]);
		lua_rawset(L, -3);
	}
}

static Object3d * getObject3d(LUA_INTEGER object)
{
	if(object == 0)
		return NULL;
	
	return (Object3d*)object;
}

static bool getVector2(lua_State * L, int index, MVector2 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 2))
	{
		lua_pushnil(L);
		
		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		return true;
	}
	
	return false;
}

static bool getVector3(lua_State * L, int index, MVector3 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 3))
	{
		lua_pushnil(L);
		
		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->z = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		return true;
	}
	
	return false;
}

static bool getVector4(lua_State * L, int index, MVector4 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 4))
	{
		lua_pushnil(L);
		
		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->z = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->w = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		return true;
	}
	
	return false;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// vector math
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vec3__add(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a+b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a+v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__sub(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a-b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a-v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__mul(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a*b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a*v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__div(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a/b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a/v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__unm(lua_State *L)
{
	MVector3 a;
	getVector3(L, 1, &a);
	pushFloatArray(L, -a, 3);
	return 1;
}

void registerVec3(lua_State * L)
{
	// register vec3
	luaL_newmetatable(L, LUA_VEC3);
	
	lua_pushcfunction(L, vec3__add); lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, vec3__sub); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, vec3__mul); lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, vec3__div); lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, vec3__unm); lua_setfield(L, -2, "__unm");
	
	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");
}

int setAttribute(lua_State* L)
{
	if(! isFunctionOk(L, "setAttribute", 3))
		return 0;
	Object3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	const char* name = lua_tostring(L, 2);
	if((object = getObject3d(id)))
	{
		int type = lua_type(L, 3);
		M_VARIABLE_TYPE varType = object->getAttribute(name).getType();
		if(object->getAttribute(name).getType() == M_VARIABLE_NULL)
		{
			switch(type)
			{
				case LUA_TNUMBER:
					{
						NeoVariable variable(name, new float(lua_tonumber(L, 3)), M_VARIABLE_FLOAT);
						object->setAttribute(name, variable);
					}
					break;
				case LUA_TSTRING:
					{
						NeoVariable variable(name, new MString(lua_tostring(L, 3)), M_VARIABLE_STRING);
						object->setAttribute(name, variable);
					}
					break;
			}
		}
		else
		{
			NeoVariable attribute = object->getAttribute(name);
			if(type == LUA_TNUMBER && attribute.getType() == M_VARIABLE_FLOAT)
			{
				*(float*)attribute.getPointer() = lua_tonumber(L, 3);
			}
			else if(type == LUA_TSTRING && attribute.getType() == M_VARIABLE_STRING)
			{
				((MString*)attribute.getPointer())->set(lua_tostring(L, 3));
			}
			else
			{
				MLOG_ERROR("setAttribute: Types do not match! Can not set attribute!");
			}
		}
	}
	return 1;
}

int getAttribute(lua_State* L)
{
	if(! isFunctionOk(L, "getAttribute", 2))
		return 0;
	Object3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	const char* name = lua_tostring(L, 2);
	if((object = getObject3d(id)))
	{
		NeoVariable variable = object->getAttribute(name);
		switch(variable.getType())
		{
			case M_VARIABLE_FLOAT:
				lua_pushnumber(L, *(float*)variable.getPointer());
				break;
			case M_VARIABLE_STRING:
				lua_pushstring(L, ((MString*)variable.getPointer())->getSafeString());
				break;
			case M_VARIABLE_NULL:
				lua_pushnil(L);
				break;
			default: MLOG_ERROR("getAttribute: Variable of type '" << variable.getType() << "' not supported!");
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LuaScript::LuaScript(void):
m_state(NULL),
m_isRunning(false)
{
	// Do this before doing anything else
	// to allow overwriting API functions.
	bindLuaApi(this);
}

LuaScript::~LuaScript(void)
{
	clear();
}

void LuaScript::init(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	
	g_startTick = system->getSystemTick();
	
	// create context
	m_state = lua_open();

	if(!m_state)
	{
		MLOG_ERROR("Could not open Lua state!");
		return;
	}

	luaL_openlibs(m_state);

	registerVec3(m_state);

    lua_register(m_state, "setAttribute",           setAttribute);
    lua_register(m_state, "getAttribute",           getAttribute);

	// register custom functions
	map<string, int (*)(void)>::iterator
		mit (m_functions.begin()),
		mend(m_functions.end());

	for(;mit!=mend;++mit)
		lua_register(m_state, mit->first.c_str(), function);
}

void LuaScript::clear(void)
{
	if(m_state)
	{
		lua_close(m_state);
		m_state = NULL;
	}
	m_isRunning = false;
}

int LuaScript::function(lua_State * L)
{
	LuaScript * script = (LuaScript *)NeoEngine::getInstance()->getScriptContext();

    lua_Debug ar;
	lua_getstack(L, 0, &ar);
    lua_getinfo(L, "n", &ar);

	map<string, int (*)(void)>::iterator iter = script->m_functions.find(ar.name);
	if(iter != script->m_functions.end())
		return iter->second();

	return 0;
}

bool LuaScript::runScript(const char * filename)
{
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
    return true;
}

bool LuaScript::startCallFunction(const char* name)
{
	if(m_isRunning)
	{
		lua_getglobal(m_state, name);
		if(!lua_isfunction(m_state, -1))
		{
			lua_pop(m_state, 1);
			return false;
		}
		return true;
	}
	return false;
}

bool LuaScript::endCallFunction(int numArgs)
{
	if(lua_pcall(m_state, numArgs, 0, 0) != 0)
	{
        MLOG_ERROR("Lua Script: \n" << lua_tostring(m_state, -1) << "\n");
		m_isRunning = false;
		return false;
	}
	return true;
}

void LuaScript::callFunction(const char * name)
{
	if(startCallFunction(name))
		endCallFunction();
}

void LuaScript::addFunction(const char * name, int (*function)(void)){
	m_functions[name] = function;
}

unsigned int LuaScript::getArgsNumber(void)
{
	return lua_gettop(m_state);
}

void LuaScript::getIntArray(unsigned int arg, int * values, unsigned int valuesNumber)
{
	arg++;
	if(lua_istable(m_state, arg) && (lua_objlen(m_state, arg) >= valuesNumber))
	{
		lua_pushnil(m_state);
		for(unsigned int i=0; i<valuesNumber; i++)
		{
			lua_next(m_state, arg);
			values[i] = (int)lua_tointeger(m_state, -1);
			lua_pop(m_state, 1);
		}
	}
}

void LuaScript::getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber)
{
	arg++;
	if(lua_istable(m_state, arg) && (lua_objlen(m_state, arg) >= valuesNumber))
	{
		lua_pushnil(m_state);
		for(unsigned int i=0; i<valuesNumber; i++)
		{
			lua_next(m_state, arg);
			values[i] = (float)lua_tonumber(m_state, -1);
			lua_pop(m_state, 1);
		}
	}
}

const char * LuaScript::getString(unsigned int arg){
	return lua_tostring(m_state, arg+1);
}

int LuaScript::getInteger(unsigned int arg){
	return (int)lua_tointeger(m_state, arg+1);
}

float LuaScript::getFloat(unsigned int arg){
	return (float)lua_tonumber(m_state, arg+1);
}

void* LuaScript::getPointer(unsigned int arg){
	return (void*)lua_tointeger(m_state, arg+1);
}

bool LuaScript::getBoolean(unsigned int arg)
{
	return lua_toboolean(m_state, arg+1);
}

void LuaScript::pushIntArray(const int * values, unsigned int valuesNumber)
{
	lua_newtable(m_state);
	for(unsigned int i=0; i<valuesNumber; i++)
	{
		lua_pushinteger(m_state, (lua_Integer)i+1);
		lua_pushinteger(m_state, (lua_Integer)values[i]);
		lua_rawset(m_state, -3);
	}

	if(valuesNumber == 3) // vec3
	{
		luaL_getmetatable(m_state, LUA_VEC3);
		lua_setmetatable(m_state, -2);
	}
}

void LuaScript::pushFloatArray(const float * values, unsigned int valuesNumber)
{
	lua_newtable(m_state);
	for(unsigned int i=0; i<valuesNumber; i++)
	{
		lua_pushinteger(m_state, (lua_Integer)i+1);
		lua_pushnumber(m_state, (lua_Number)values[i]);
		lua_rawset(m_state, -3);
	}

	if(valuesNumber == 3) // vec3
	{
		luaL_getmetatable(m_state, LUA_VEC3);
		lua_setmetatable(m_state, -2);
	}
}

void LuaScript::pushBoolean(bool value){
	lua_pushboolean(m_state, (int)value);
}

void LuaScript::pushString(const char * string){
	lua_pushstring(m_state, string);
}

void LuaScript::pushInteger(int value){
	lua_pushinteger(m_state, (lua_Integer)value);
}

void LuaScript::pushFloat(float value){
	lua_pushnumber(m_state, (lua_Number)value);
}

void LuaScript::pushPointer(void* value){
	lua_pushinteger(m_state, (lua_Integer)value);
}

bool LuaScript::isNumber(unsigned int arg)
{
	return lua_isnumber(m_state, arg+1);
}

bool LuaScript::runString(const char* str)
{
    if(!m_state)
        init();

	// do string
	if(luaL_dostring(m_state, str) != 0)
	{
		MLOG_ERROR("Lua Script: \n" << lua_tostring(m_state, -1) << "\n");
		return false;
	}

	return true;
}

bool LuaScript::isFunctionOk(const char* function, unsigned int argc)
{
	if(getArgsNumber() < argc)
	{
		MLOG_ERROR("'" << function << "' needs at least " << argc << " parameters!");
		return false;
	}

	return true;
}

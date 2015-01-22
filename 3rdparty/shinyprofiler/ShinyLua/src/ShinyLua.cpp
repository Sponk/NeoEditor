/*
The MIT License

Copyright (c) 2007-2010 Aidin Abedi http://code.google.com/p/shinyprofiler/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "ShinyLua.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <string.h>
#include <string>

// Note - not in the original STL, but exists in SGI STL and STLport
#if (SHINY_COMPILER == SHINY_COMPILER_GNUC) && !defined(STLPORT)
#   include <tr1/unordered_map>
#   include <tr1/unordered_set>
#else
#   include <hash_set>
#   include <hash_map>
#endif

#if SHINY_COMPILER == SHINY_COMPILER_GNUC && __GNUC__ >= 3 && __GNUC_MINOR__ >= 1 && !defined(STLPORT)
#   define HASHMAP std::tr1::unordered_map
#else
#   if SHINY_COMPILER == SHINY_COMPILER_MSVC
#       if _MSC_VER > 1300 && !defined(_STLP_MSVC)
#           define HASHMAP ::stdext::hash_map
#       else
#           define HASHMAP ::std::hash_map
#       endif
#   else
#       define HASHMAP ::std::hash_map
#   endif
#endif


/*---------------------------------------------------------------------------*/

struct Profile {
	ShinyZone zone;
	ShinyNodeCache cache;
	std::string name;

	Profile(void) {
		ShinyZone_clear(&zone);
		cache = &_ShinyNode_dummy;
	}
};

typedef HASHMAP<const void*, Profile> ProfileMap;


/*---------------------------------------------------------------------------*/

int is_running = 0;

ProfileMap profiles;


/*---------------------------------------------------------------------------*/

std::string StringPrintV(const char* format, va_list args) {
#if (SHINY_COMPILER == SHINY_COMPILER_MSVC)
	int size = _vscprintf(format, args);
	std::string buffer;
	buffer.resize(size);

	vsnprintf(&buffer[0], size, format, args);
	return buffer;

#else
	char* buffer;

	vasprintf(&buffer, format, args);

	std::string str(buffer);
	free(buffer);
	return str;
#endif
}


/*---------------------------------------------------------------------------*/

std::string StringPrint(const char* format, ...) {
	va_list args;

	va_start(args, format);
	std::string str(StringPrintV(format, args));
	va_end(args);

	return str;
}

/*---------------------------------------------------------------------------*/

Profile* FindProfile(lua_State *L, lua_Debug *ar) {
	const void *func = NULL;

	lua_getinfo(L, "f", ar);
	func = lua_topointer(L, -1);
	lua_pop(L, 1);

	Profile *prof = &profiles[func];

	if (!prof->zone.name) {
		lua_getinfo(L, "S", ar);

		switch (ar->what[0]) {
			case 'L': // "Lua"
				prof->name =
					StringPrint("%s(%d):%s", ar->source, ar->linedefined, ar->name);
				break;

			case 'C': // "C"
				prof->name = "C:";
				prof->name += ar->name;
				break;

			default: // impossible happened
				prof->name = "<unknown>";
		}

		prof->zone.name = prof->name.c_str();
	}

	return prof;
}


/*---------------------------------------------------------------------------*/

void callhook(lua_State *L, lua_Debug *ar) {
	// ignore tail call
	if (ar->i_ci == 0 || ar->event == LUA_HOOKTAILRET) return;

	// ignore nameless function
	lua_getinfo(L, "n", ar);
	if (!ar->name) return;

	if (ar->event == LUA_HOOKCALL) {
		Profile *prof = FindProfile(L, ar);
		ShinyManager_lookupAndBeginNode(&Shiny_instance, &prof->cache, &prof->zone);
		return;

	} else { // LUA_HOOKRET
		ShinyManager_endCurNode(&Shiny_instance);
		return;
	}
}


/*---------------------------------------------------------------------------*/

int ShinyLua_update(lua_State *L) {
	PROFILE_UPDATE();
	return 0;
}


/*---------------------------------------------------------------------------*/

int ShinyLua_clear(lua_State *L) {
	PROFILE_CLEAR();
	return 0;
}


/*---------------------------------------------------------------------------*/

int ShinyLua_damping(lua_State *L) {
	if (lua_gettop(L) == 1) {
		float damping = (float) luaL_checknumber(L, -1);
		PROFILE_SET_DAMPING(damping);
		return 0;

	} else {
		lua_pushnumber(L, PROFILE_GET_DAMPING());
		return 1;
	}
}


/*---------------------------------------------------------------------------*/

int ShinyLua_start(lua_State *L) {
	if (is_running) return 0;
	is_running = 1;
	lua_sethook(L, callhook, LUA_MASKCALL | LUA_MASKRET, 0);
	return 0;
}


/*---------------------------------------------------------------------------*/

int ShinyLua_stop(lua_State *L) {
	if (!is_running) return 0;
	is_running = 0;
	lua_sethook(L, callhook, 0, 0);
	return 0;
}


/*---------------------------------------------------------------------------*/

int ShinyLua_is_running(lua_State *L) {
	lua_pushboolean(L, is_running);
	return 1;
}



/*---------------------------------------------------------------------------*/

int ShinyLua_output(lua_State *L) {
	const char* outfile = NULL;

	if (lua_gettop(L) == 1)
		outfile = luaL_checkstring(L, -1);

	PROFILE_OUTPUT(outfile);

	return 0;
}


/*---------------------------------------------------------------------------*/

int ShinyLua_tree_string(lua_State *L) {
	lua_pushstring(L, PROFILE_GET_TREE_STRING().c_str());
	return 1;
}


/*---------------------------------------------------------------------------*/

int ShinyLua_flat_string(lua_State *L) {
	lua_pushstring(L, PROFILE_GET_FLAT_STRING().c_str());
	return 1;
}


/*---------------------------------------------------------------------------*/

int luaopen_ShinyLua(lua_State *L) {
	const luaL_reg funcs[] = {
		{ "update", ShinyLua_update },
		{ "clear", ShinyLua_clear },
		{ "output", ShinyLua_output },
		{ "damping", ShinyLua_damping },
		{ "start", ShinyLua_start },
		{ "stop", ShinyLua_stop },
		{ "is_running", ShinyLua_is_running },
		{ "tree_string", ShinyLua_tree_string },
		{ "flat_string", ShinyLua_flat_string },
		{ NULL, NULL }
	};

	luaL_openlib(L, "shiny", funcs, 0);

	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, SHINY_COPYRIGHT);
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, SHINY_DESCRIPTION);
	lua_settable (L, -3);
	lua_pushliteral (L, "_NAME");
	lua_pushliteral (L, SHINY_FULLNAME);
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, SHINY_VERSION);
	lua_settable (L, -3);
	return 1;
}

#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>

#ifndef WIN32
#include "opendlg-gtk.c"
#else
#include "opendlg-win32.c"
#endif

static const luaL_Reg luacommondlg[] = {
	{"getOpenFilename", getOpenFilename},
	{"getSaveFilename", getSaveFilename},
	{NULL, NULL}
};

LUALIB_API int luaopen_LuaCommonDlg (lua_State *L)
{
	luaL_register(L, "LuaCommonDlg", luacommondlg);
	initToolkit();
	return 1;
}

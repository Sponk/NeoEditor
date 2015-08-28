#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>

#if (!defined(WIN32) && defined(GTK_BACKEND))
#include "opendlg-gtk.h"
#elif (!defined(WIN32) && defined(QT_BACKEND))
#include "opendlg-qt.h"
#elif defined(WIN32)
#include "opendlg-win32.h"
#elif defined(NFD_BACKEND)
int getOpenFilename(lua_State* L);
int getSaveFilename(lua_State* L);
void initToolkit();
#else

int getOpenFilename(lua_State* L)
{
	return 0;
}


int getSaveFilename(lua_State* L)
{
	return 0;
}

void initToolkit()
{

}

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

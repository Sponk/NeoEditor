#include <nfd.h>
#include <lua.h>

int getOpenFilename(lua_State* L)
{
	nfdchar_t* filename = NULL;
	nfdresult_t result = NFD_OpenDialog(NULL, NULL, &filename);

	if(result == NFD_OKAY)
	{
		lua_pushstring(L, filename);
		free(filename);
		return 1;
	}

	return 0;
}

int getSaveFilename(lua_State* L)
{
	nfdchar_t* filename = NULL;
	nfdresult_t result = NFD_SaveDialog(NULL, NULL, &filename);

	if(result == NFD_OKAY)
	{
		lua_pushstring(L, filename);
		free(filename);
		return 1;
	}

	return 0;
}

void initToolkit()
{

}

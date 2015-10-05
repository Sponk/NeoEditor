#include <windows.h>
#include <lua.h>
#include <lauxlib.h>

int getOpenFilename(lua_State* L)
{
	OPENFILENAME ofn;
	char szFile[255];
	char szFilter[255];
	char szStart[255];

	strcpy(szStart, lua_tostring(L, 1));
	strcpy(szFilter, lua_tostring(L, 2));

	ZeroMemory(&ofn , sizeof( ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = szStart;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileName(&ofn);

	if(szFile[0] != '\0')
		lua_pushstring(L, szFile);
	else
		lua_pushnil(L);

	return 1;
}

int getSaveFilename(lua_State* L)
{
	OPENFILENAME ofn;
	char szFile[255];
	char szFilter[255];
	char szStart[255];

	strcpy(szStart, lua_tostring(L, 1));
	strcpy(szFilter, lua_tostring(L, 2));

	ZeroMemory(&ofn , sizeof( ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	//ofn.lpstrFilter = szFilter;
	//ofn.nFilterIndex = 1;
	//ofn.lpstrFileTitle = NULL;
	//ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = szStart;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = szFilter;

	GetSaveFileName(&ofn);

	if(szFile[0] != '\0')
		lua_pushstring(L, szFile);
	else
		lua_pushnil(L);

	return 1;
}

void initToolkit()
{

}

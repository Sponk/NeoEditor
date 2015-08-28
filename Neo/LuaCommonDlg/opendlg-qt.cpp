#include <lua.hpp>
#include <Qt/qfiledialog.h>
#include <Qt/qapplication.h>

extern "C" {

int argc = 1;
char* argv[] = {"FUCKYOUQT"};
QApplication qapp(argc, argv);

int getOpenFilename(lua_State* L)
{
	const char* start = lua_tostring(L, 1);

	const char* filename = QFileDialog::getOpenFileName(NULL, "Open File", start, "All Files (*)").toUtf8();
	lua_pushstring(L, filename);
	return 1;
}

int getSaveFilename(lua_State* L)
{

}

void initToolkit()
{

}

}

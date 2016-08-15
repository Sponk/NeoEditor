#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#define SEXPRESSO_OPT_OUT_PIKESTYLE
#include <sexpresso.hpp>

using namespace std;
std::string& convertQtFilterToWin32(std::string& str)
{
	// First, remove ";;" stuff from Qt
    str.erase(std::remove(str.begin(), str.end(), ';'), str.end());

	// Replace bracket with '\0' just how windows likes it
    std::replace(str.begin(), str.end(), ',', ';'); // replace all ',' with ';'
    std::replace(str.begin(), str.end(), '(', '#'); // replace all '(' with '#'
	std::replace(str.begin(), str.end(), ')', '#'); // replace all ')' with '#'
	std::replace(str.begin(), str.end(), '#', '\0'); // replace all '#' with '\n'

	return str;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	stringstream ss;
	ss << cin.rdbuf();

	auto config = sexpresso::parse(ss.str());

	std::string directory = config.getChildByPath("directories")->arguments().begin()->value.str;
	std::string save = config.getChildByPath("save")->arguments().begin()->value.str;

	char szFile[256]; // The filename
	OPENFILENAME lpofn;
	ZeroMemory(&lpofn, sizeof(OPENFILENAME));
	lpofn.lStructSize = sizeof(OPENFILENAME);
	lpofn.lpstrTitle = config.getChildByPath("message")->arguments().begin()->value.str.c_str();
	lpofn.lpstrFilter = convertQtFilterToWin32(config.getChildByPath("filetypes")->arguments().begin()->value.str).c_str();
	lpofn.lpstrInitialDir = config.getChildByPath("startdir")->arguments().begin()->value.str.c_str();
    lpofn.nFilterIndex = 1;
	lpofn.lpstrFile = szFile;
	lpofn.lpstrFile[0] = 0;
	lpofn.nMaxFile = sizeof(szFile);

	DWORD flags = 0;
	if(directory == "1" || directory == "true")
		flags |= OFN_PATHMUSTEXIST;

	bool saving = false;
	if(save == "1" || save == "true")
	{
		saving = true;
		flags |= OFN_OVERWRITEPROMPT;
	}
	else
		flags |= OFN_FILEMUSTEXIST;

	lpofn.Flags = flags | OFN_NOCHANGEDIR;

	if(saving)
		GetSaveFileName(&lpofn);
	else
		GetOpenFileName(&lpofn);

	cout << "(filename \"" << lpofn.lpstrFile << "\")";
	return 0;
}

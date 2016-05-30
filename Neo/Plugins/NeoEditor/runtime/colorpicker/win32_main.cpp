#include <windows.h>
#include <iostream>
#include <sstream>

#define SEXPRESSO_OPT_OUT_PIKESTYLE
#include <sexpresso.hpp>

using namespace std;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::string line;
	stringstream ss;
	ss << cin.rdbuf();

	auto config = sexpresso::parse(ss.str());

	std::string title = config.getChildByPath("title")->arguments().begin()->value.str;
	std::string text = config.getChildByPath("text")->arguments().begin()->value.str;

	MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
	return 0;
}
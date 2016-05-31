#include <windows.h>
#include <iostream>
#include <sstream>

#define SEXPRESSO_OPT_OUT_PIKESTYLE
#include <sexpresso.hpp>

using namespace std;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	stringstream ss;
	ss << cin.rdbuf();

	auto config = sexpresso::parse(ss.str());
	auto comp = config.getChildByPath("color")->arguments().begin();
	float r = stof(comp->value.str);
	float g = stof((++comp)->value.str);
	float b = stof((++comp)->value.str);
	float a = stof((++comp)->value.str);

	COLORREF acrCustClr[16];
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(cc));

	COLORREF result = RGB(r * 255, g * 255, b * 255);
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = result;
	cc.lpCustColors = acrCustClr;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	ChooseColor(&cc);

	cout << "(color " << static_cast<float>(GetRValue(cc.rgbResult)) / 255.0f
		<< " " << static_cast<float>(GetGValue(cc.rgbResult)) / 255.0f
		<< " " << static_cast<float>(GetBValue(cc.rgbResult)) / 255.0f << " " << a << ")";

	return 0;
}
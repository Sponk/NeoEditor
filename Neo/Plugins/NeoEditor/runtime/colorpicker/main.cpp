#include <QColorDialog>
#include <QApplication>
#include <iostream>
#include <sstream>
#include <sexpresso.hpp>

using namespace std;

int main(int argc, char* argv[])
{
	stringstream ss;
	ss << cin.rdbuf();

	auto config = sexpresso::parse(ss.str());

	auto comp = config.getChildByPath("color")->arguments().begin();
	float r = stof(comp->value.str);
	float g = stof((++comp)->value.str);
	float b = stof((++comp)->value.str);
	float a = stof((++comp)->value.str);

	QApplication app(argc, argv);
	QColor c = QColorDialog::getColor(QColor(r*255, g*255, b*255, a*255));
	cout << "(color " << static_cast<float>(c.red()) / 255.0f
		 << " " << static_cast<float>(c.green()) / 255.0f
		 << " " << static_cast<float>(c.blue()) / 255.0f
		 << " " << static_cast<float>(c.alpha()) / 255.0f << ")";
	return 0;
}

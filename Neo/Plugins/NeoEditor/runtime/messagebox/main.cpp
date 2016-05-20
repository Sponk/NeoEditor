#include <QMessageBox>
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

	QApplication app(argc, argv);

	std::string title = config.getChildByPath("title")->arguments().begin()->value.str;
	std::string text = config.getChildByPath("text")->arguments().begin()->value.str;

	QMessageBox::information(NULL, title.c_str(), text.c_str());
	return 0;
}
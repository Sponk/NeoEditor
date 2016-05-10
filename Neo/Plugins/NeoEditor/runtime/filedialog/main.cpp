#include <QFileDialog>
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

	std::string directory = config.getChildByPath("directories")->arguments().begin()->value.str;
	std::string save = config.getChildByPath("save")->arguments().begin()->value.str;

	QString fileName;

	if(directory == "1" || directory == "true")
		fileName = QFileDialog::getExistingDirectory(nullptr,
													 config.getChildByPath("message")->arguments().begin()->value.str.c_str(),
													 config.getChildByPath("startdir")->arguments().begin()->value.str.c_str());
	else
		if(save == "1" || save == "true")
			fileName = QFileDialog::getSaveFileName(nullptr,
										 config.getChildByPath("message")->arguments().begin()->value.str.c_str(),
										 config.getChildByPath("startdir")->arguments().begin()->value.str.c_str(),
										 config.getChildByPath("filetypes")->arguments().begin()->value.str.c_str());

		else
			fileName = QFileDialog::getOpenFileName(nullptr,
												config.getChildByPath("message")->arguments().begin()->value.str.c_str(),
												config.getChildByPath("startdir")->arguments().begin()->value.str.c_str(),
												config.getChildByPath("filetypes")->arguments().begin()->value.str.c_str());


	cout << "(filename \"" << fileName.toUtf8().data() << "\")" << endl;
}
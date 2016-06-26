#include <QMessageBox>
#include <QApplication>
#include <QDialog>
#include <iostream>
#include <sstream>
#include <sexpresso.hpp>
#include "dialog.h"

using namespace std;

int main(int argc, char* argv[])
{
	stringstream ss;
	ss << cin.rdbuf();

	auto config = sexpresso::parse(ss.str());
	QStringList entries;
	QString windowTitle;

	auto titleNode = config.getChildByPath("title");
	auto entriesNode = config.getChildByPath("entries");

	if(!titleNode || !entriesNode)
	{
		cout << "(result)" << endl;
		return 0;
	}

	windowTitle = titleNode->arguments().begin()->value.str.c_str();
	for(auto n : entriesNode->arguments())
		entries << n.value.str.c_str();

	QApplication app(argc, argv);
	Dialog dlg(entries, nullptr);
	dlg.setWindowTitle(windowTitle);

	dlg.show();
	int retval = app.exec();
	
	cout << "(result \"" << dlg.getSelection().toStdString() << "\")" << endl;
	return retval;
}

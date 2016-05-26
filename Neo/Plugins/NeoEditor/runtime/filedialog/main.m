#include <iostream>
#include <sstream>
#include <vector>
#include <sexpresso.hpp>

#define GNUSTEP
#import <AppKit/AppKit.h>

using namespace std;

int main(int argc, char* argv[])
{
	stringstream ss;
	ss << cin.rdbuf();

	auto config = sexpresso::parse(ss.str());

	std::string directory = config.getChildByPath("directories")->arguments().begin()->value.str;
	std::string save = config.getChildByPath("save")->arguments().begin()->value.str;

	NSApplication *app;
	NSAutoreleasePool *pool;

	pool = [NSAutoreleasePool new];
	app = [NSApplication sharedApplication];

	std::string fileName;
	if(directory == "1" || directory == "true")
	{
		NSOpenPanel* openDlg = [NSOpenPanel openPanel];
		[openDlg canChooseFiles:false];
		[openDlg canChooseDirectories:true];
		[openDlg setPrompt:[NSString stringWithCString:config.getChildByPath("message")->arguments().begin()->value.str.c_str() encoding:NSUTF8StringEncoding]];
		[openDlg runModal];

				//fileName = QFileDialog::getExistingDirectory(nullptr,
			//										 config.getChildByPath("message")->arguments().begin()->value.str.c_str(),
				//									 config.getChildByPath("startdir")->arguments().begin()->value.str.c_str());
	}
				/*else
		if(save == "1" || save == "true")
			fileName = QFileDialog::getSaveFileName(nullptr,
										 config.getChildByPath("message")->arguments().begin()->value.str.c_str(),
										 config.getChildByPath("startdir")->arguments().begin()->value.str.c_str(),
										 config.getChildByPath("filetypes")->arguments().begin()->value.str.c_str());

		else
			fileName = QFileDialog::getOpenFileName(nullptr,
												config.getChildByPath("message")->arguments().begin()->value.str.c_str(),
												config.getChildByPath("startdir")->arguments().begin()->value.str.c_str(),
												config.getChildByPath("filetypes")->arguments().begin()->value.str.c_str());*/



//	cout << "(filename \"" << fileName.toUtf8().data() << "\")" << endl;
}

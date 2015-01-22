
#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_message.H>

#include <string>

#include "NeoStore.h"
#include "RepositoryManager.h"
#include "Downloader.h"

#include <iostream>
#include <sstream>

using namespace std;

// Don't show cmd window
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

//#define DEFAULT_HOST "127.0.0.1"

int main(int argc, char **argv)
{
	Fl::background(0xEE,0xEE,0xEE);
	Fl::background2(0xFF,0xFF,0xFF);

	//Fl::get_system_colors();
    Fl::scheme("gtk+");

#ifdef WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup failed: " << WSAGetLastError() << endl;
		return 1;
	}
#endif

	NeoStore dlg;
    Fl_Double_Window* window = dlg.create_window();

	RepositoryManager* repoMgr = RepositoryManager::getInstance();
	Repository mainRepo;
	mainRepo.setServer(DEFAULT_HOST);

	repoMgr->addRepository(mainRepo);
	repoMgr->updatePackageInformation();

	vector<Repository::Package> packages;
	repoMgr->getPackageList(&packages);

	for(int i = 0; i < packages.size(); i++)
	{
		Repository::Package* p = (Repository::Package*) malloc(sizeof(Repository::Package));
		new(p) Repository::Package();

		*p = packages[i];
		dlg.available_packages->add(packages[i].name.c_str(), (void*) p);
		cout << "Got package: " << packages[i].name << endl;
	}

	window->show();
    return Fl::run();
}

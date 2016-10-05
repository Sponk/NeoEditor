#include "NPKPublisher.h"
#include <NeoEngine.h>
#include <Project.h>
#include <iostream>

using namespace std;
using namespace Neo;

static Publish::NPKPublisher publisher;
static Publish::PublisherRegistry reg(publisher);

bool Publish::NPKPublisher::createPackage(const char* src, const char* dest, const char* project, const char* pwd, bool verbose)
{
	Plugin npkPlugin;
	npkPlugin.load("NPKPlugin");

	NeoEngine* engine = NeoEngine::getInstance();
	PackageManager* pkm = engine->getPackageManager();

	if(!pkm)
	{
		cout << "No suitable package manager found!" << endl;
		return false;
	}
	
	// Packaging!
	{
		cout << "Opening package... ";
		Package package = pkm->openPackage(dest);

		if (package)
			cout << "Done." << endl;
		else
		{
			cout << "Could not open package!" << endl;
			return false;
		}

		// const char* pwd = engine->getSystemContext()->getWorkingDirectory();

		packageDir(src, pwd, pkm, package, verbose);
		pkm->addFileToPackage(project, package, "project.nproj");

		cout << "Saving package... ";
		pkm->closePackage(package);
		cout << "Done." << endl;
	}
	
	return true;
}


bool Publish::NPKPublisher::publish(const char* projectFile,
									const char* executable,
									const char* output,
									bool verbose,
									const char* levelOverride,
									std::function<void(int)> progressCallback)
{
	bool success = true;
	char execdir[256];
	getRepertory(execdir, executable);

	char projectdir[256];
	getRepertory(projectdir, projectFile);
	
	string execstr = execdir;
	string outpath = output;
	outpath += "/";
	
	Project project;
	if(!project.load(projectFile))
	{
		perror("Could not load project file");
		return false;
	}
	
	// Packaging!
	{
		std::string packagefile = output;
		packagefile += SEPARATOR;
		packagefile += "assets.npk";

		if (!isFileExist(output) && !createDirectory(output, true))
		{
			perror("Could not create output directory");
			return false;
		}

		success &= createPackage(project.getAssetDirectory().c_str(), packagefile.c_str(), projectFile, projectdir, verbose);
	}
	
	return success;
}

bool Publish::NPKPublisher::packageDir(const char* dir, const char* pwd, Neo::PackageManager* pkm, Neo::Package pkg, bool verbose)
{
	vector<string> files;
	readDirectory(dir, files, true, true);

	char localFilename[256];
	for(auto f : files)
	{
		getLocalFilename(localFilename, pwd, f.c_str());

		if(verbose)
			cout << "Packaging " << f << " to " << localFilename << endl;

		pkm->addFileToPackage(f.c_str(), pkg, localFilename);
	}

	return true;
}

bool Publish::NPKPublisher::copyFiles(const char* src, const char* dest, const char* type, bool verbose)
{
	vector<string> files;
	readDirectory(src, files, true, false);

	bool success = true;
	char localFilename[256];
	int typelen = strlen(type);
	string destination = dest;
	string source = src;

	for (auto f : files)
	{
		if (typelen && f.find(type) == -1)
			continue;
				
		if (verbose)
			cout << "Copying " << src << f << " to " << destination + f << endl;

		success &= copyFile((source + f).c_str(), (destination + f).c_str());
	}

	return success;
}

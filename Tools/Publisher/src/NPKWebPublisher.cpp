#include "NPKWebPublisher.h"
#include <NeoEngine.h>
#include <Project.h>
#include <iostream>

using namespace std;
using namespace Neo;

static Publish::NPKWebPublisher publisher;
static Publish::PublisherRegistry reg(publisher);

bool Publish::NPKWebPublisher::publish(const char* projectFile,
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

		success &= createPackage((projectdir + project.getAssetDirectory()).c_str(), packagefile.c_str(), projectFile, projectdir, verbose);
	}
	
	{
		cout << "Copying runtime components... " << endl;
		success &= copyFiles(execdir, outpath.c_str(), "html", verbose);
		success &= copyFiles(execdir, outpath.c_str(), "data", verbose);
		success &= copyFiles(execdir, outpath.c_str(), "mem", verbose);
		success &= copyFiles(execdir, outpath.c_str(), "js", verbose);
		cout << "Done." << endl;
	}
	
	return success;
}

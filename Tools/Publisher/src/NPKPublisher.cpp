#include "NPKPublisher.h"
#include <NeoEngine.h>
#include <Project.h>
#include <iostream>

#ifdef WIN32
#define SEPARATOR "\\"
#else
#include <sys/stat.h>

#define SEPARATOR "/"
#endif

using namespace std;
using namespace Neo;

static Publish::NPKPublisher publisher;
static Publish::PublisherRegistry reg(publisher);

bool Publish::NPKPublisher::publish(const char* projectFile,
									const char* executable,
									const char* output,
									bool verbose,
									const char* levelOverride,
									std::function<void(int)> progressCallback)
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

	Project project;
	if(!project.load(projectFile))
	{
		perror("Could not load project file");
		return false;
	}

	std::string packagefile = output;
	packagefile += SEPARATOR;
	packagefile += "assets.npk";

	if(!isFileExist(output) && !createDirectory(output, true))
	{
		perror("Could not create output directory");
		return false;
	}

	Package package = pkm->openPackage(packagefile.c_str());
	const char* pwd = engine->getSystemContext()->getWorkingDirectory();

	packageDir(project.getAssetDirectory().c_str(), pwd, pkm, package, verbose);
	pkm->addFileToPackage(projectFile, package, "project.nproj");

	bool success = true;

#ifdef WIN32

#elif LINUX
	packagefile = output;
	packagefile += SEPARATOR;

	{
		string strexec = (packagefile + project.getName());
		success &= copyFile(executable, strexec.c_str());

		// Make executable!
		chmod(strexec.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	}

	char execdir[256];
	getRepertory(execdir, executable);

	string execstr = execdir;
	for(auto str : project.getPlugins())
	{
		if(verbose)
			cout << "Copying " << (execstr + str + ".so") << " to " << (packagefile + str + ".so") << endl;

		success &= copyFile((execstr + str + ".so").c_str(), (packagefile + str + ".so").c_str());
	}

	success &= copyFile((execstr + "NeoEngine.so").c_str(), (packagefile + "NeoEngine.so").c_str());
	success &= copyFile((execstr + "NeoCore.so").c_str(), (packagefile + "NeoCore.so").c_str());
	success &= copyFile((execstr + "Neo2D.so").c_str(), (packagefile + "Neo2D.so").c_str());
	success &= copyFile((execstr + "NeoEngineLua.so").c_str(), (packagefile + "NeoEngineLua.so").c_str());
	success &= copyFile((execstr + "Neo2DLua.so").c_str(), (packagefile + "Neo2DLua.so").c_str());

#elif APPLE

#endif

	pkm->closePackage(package);
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
}
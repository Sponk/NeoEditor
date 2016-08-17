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

		cout << "Opening package... ";
		Package package = pkm->openPackage(packagefile.c_str());

		if (package)
			cout << "Done." << endl;
		else
		{
			cout << "Could not open package!" << endl;
			return false;
		}

		const char* pwd = engine->getSystemContext()->getWorkingDirectory();

		packageDir(project.getAssetDirectory().c_str(), pwd, pkm, package, verbose);
		pkm->addFileToPackage(projectFile, package, "project.nproj");

		cout << "Saving package... ";
		pkm->closePackage(package);
		cout << "Done." << endl;
	}

	bool success = true;
	char execdir[256];
	getRepertory(execdir, executable);

	string execstr = execdir;

	string outpath = output;
	outpath += "/";

	cout << "Copying additional files... ";
	{
		char path[256];

		for (auto str : project.getAdditionalFiles())
		{
			if (verbose)
				cout << "Copying " << (execstr + str) << " to " << (outpath + str) << endl;

			string outfile = (outpath + str);
			getRepertory(path, outfile.c_str());

			createDirectory(path, true);
			success &= copyFile((execstr + str).c_str(), outfile.c_str());
		}
	}
	cout << "Done." << endl;

#ifdef WIN32

	cout << "Copying runtime components... ";
	success &= copyFiles(execdir, outpath.c_str(), "dll", verbose);

	{
		string strexec = (outpath + project.getName() + ".exe");
		success &= copyFile(executable, strexec.c_str());
	}
	cout << "Done." << endl;
		
	/*for (auto str : project.getPlugins())
	{
		if (verbose)
			cout << "Copying " << (execstr + str + ".dll") << " to " << (packagefile + str + ".dll") << endl;

		success &= copyFile((execstr + str + ".dll").c_str(), (packagefile + str + ".dll").c_str());
	}*/
		
	/*success &= copyFile((execstr + "NeoEngine.dll").c_str(), (packagefile + "NeoEngine.dll").c_str());
	success &= copyFile((execstr + "NeoCore.dll").c_str(), (packagefile + "NeoCore.dll").c_str());
	success &= copyFile((execstr + "Neo2D.dll").c_str(), (packagefile + "Neo2D.dll").c_str());
	success &= copyFile((execstr + "NeoEngineLua.dll").c_str(), (packagefile + "NeoEngineLua.dll").c_str());
	success &= copyFile((execstr + "Neo2DLua.dll").c_str(), (packagefile + "Neo2DLua.dll").c_str());*/

#elif LINUX

	cout << "Copying runtime components... ";
	success &= copyFiles(execdir, outpath.c_str(), "so", verbose);

	{
		string strexec = (outpath + project.getName());
		success &= copyFile(executable, strexec.c_str());

		// Make executable!
		chmod(strexec.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	}

	cout << "Done." << endl;

	/*for(auto str : project.getPlugins())
	{
		if(verbose)
			cout << "Copying " << (execstr + str + ".so") << " to " << (outpath + str + ".so") << endl;

		success &= copyFile((execstr + str + ".so").c_str(), (outpath + str + ".so").c_str());
	}

	success &= copyFile((execstr + "NeoEngine.so").c_str(), (outpath + "NeoEngine.so").c_str());
	success &= copyFile((execstr + "NeoCore.so").c_str(), (outpath + "NeoCore.so").c_str());
	success &= copyFile((execstr + "Neo2D.so").c_str(), (outpath + "Neo2D.so").c_str());
	success &= copyFile((execstr + "NeoEngineLua.so").c_str(), (outpath + "NeoEngineLua.so").c_str());
	success &= copyFile((execstr + "Neo2DLua.so").c_str(), (outpath + "Neo2DLua.so").c_str());*/

#elif APPLE

#endif

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

	for (auto f : files)
	{
		if (typelen && f.find(type) == -1)
			continue;
				
		if (verbose)
			cout << "Copying " << f << " to " << destination + f << endl;

		success &= copyFile(f.c_str(), (destination + f).c_str());
	}

	return success;
}
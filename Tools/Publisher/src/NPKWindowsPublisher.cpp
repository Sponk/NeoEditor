#include "NPKWindowsPublisher.h"
#include <Project.h>
#include <NeoEngine.h>
#include <iostream>

#ifdef LINUX
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace Neo;
using namespace std;

static Publish::NPKWindowsPublisher publisher;
static Publish::PublisherRegistry reg(publisher);

Publish::NPKWindowsPublisher::NPKWindowsPublisher()
{

}

bool Publish::NPKWindowsPublisher::publish(const char* projectFile,
						const char* executable,
						const char* output,
						bool verbose,
						const char* levelOverride,
						std::function<void(int)> progressCallback)
{
	bool success = true;
	char execdir[256];
	getRepertory(execdir, executable);

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

		success &= createPackage(project.getAssetDirectory().c_str(), packagefile.c_str(), projectFile, verbose);
	}
	
	// Runtime
	{
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
	}
	
	return success;
}

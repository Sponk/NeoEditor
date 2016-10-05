#include "NPKLinuxPublisher.h"
#include <Project.h>
#include <NeoEngine.h>
#include <iostream>

#ifdef LINUX
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace Neo;
using namespace std;

static Publish::NPKLinuxPublisher publisher;
static Publish::PublisherRegistry reg(publisher);

Publish::NPKLinuxPublisher::NPKLinuxPublisher()
{

}

bool Publish::NPKLinuxPublisher::publish(const char* projectFile,
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
	
	// Runtime
	{
		cout << "Copying runtime components... " << endl;
		success &= copyFiles(execdir, outpath.c_str(), "so", verbose);

		{
			string strexec = (outpath + project.getName());
			success &= copyFile(executable, strexec.c_str());

	#ifdef LINUX
			// Make executable!
			chmod(strexec.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	#endif
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
	}
	
	return success;
}

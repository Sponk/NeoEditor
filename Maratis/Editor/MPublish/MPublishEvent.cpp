/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPublishEvent.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2012 Philipp Geyer <nistur.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

#include <MEngine.h>
#include <MGui.h>
#include <MProject/MProject.h>

#include "MPublisher.h"
#include "MPublishEvent.h"

#include "../MBins/MMeshBin.h"

#include <sys/stat.h>

#ifdef linux
	#include <sys/types.h>
#endif


// some helper functions for repetitive packaging work
static string s_pubDir;
static string s_dataDir;

const char* getPubDir()
{
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char dir[256];
	getGlobalFilename(dir, system->getWorkingDirectory(), "published");
	s_pubDir = dir;

	return s_pubDir.c_str();
}

const char* getDataDir()
{
	if(s_dataDir.empty())
		return getPubDir();

	return s_dataDir.c_str();
}

MPackage openProjectPackage(const char* projName)
{
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char projFile[256];
	getLocalFilename(projFile, system->getWorkingDirectory(), projName);
	char* ext = strstr(projFile, ".mproj");
	if(ext == 0)
	{
		return 0;
	}

	sprintf(ext, ".npk");
	char packFile[256];

	getGlobalFilename(packFile, getDataDir(), projFile);

	MPackageManager* packageManager = engine->getPackageManager();
	MPackage package = packageManager->openPackage(packFile);

	return package;
}

/*--------------------------------------------------------------------------------
 * MPublishEventClearDirectory
 * Will clear the directory before publishing to it.
 *-------------------------------------------------------------------------------*/
class MPublishEventClearDirectory : public MPublishEvent
{
	void	execute(const char* projName)
	{
		removeDirectory(getPubDir());
		createDirectory(getPubDir());
	}

	int		getPriority() { return 0; }
};

M_PUBLISH_EVENT_IMPLEMENT(MPublishEventClearDirectory);

/*--------------------------------------------------------------------------------
 * M_PUBLISH_PACKAGE_DIR
 * Simple packaging. Will just package the whole directory using the registered
 * Package manager
 *-------------------------------------------------------------------------------*/
#define M_PUBLISH_PACKAGE_DIR(dir) \
class MPublishEvent##dir##Package : public MPublishEvent \
{ \
	void	execute(const char* projName) \
	{ \
		MEngine* engine = MEngine::getInstance(); \
		MSystemContext* system = engine->getSystemContext(); \
		char directory[256], localFilename[256]; \
		getGlobalFilename(directory, system->getWorkingDirectory(), #dir); \
		vector<string> files; \
		readDirectory(directory, &files, 1, 1); \
		MPackage package = openProjectPackage(projName); \
		MPackageManager* packageManager = engine->getPackageManager();\
		for(int i = 0; i < files.size(); ++i) \
		{ \
			getLocalFilename(localFilename, system->getWorkingDirectory(), files[i].c_str()); \
			packageManager->addFileToPackage(files[i].c_str(), package, localFilename); \
		} \
		packageManager->closePackage(package); \
	} \
	int		getPriority() { return 5; } \
}; \
M_PUBLISH_EVENT_IMPLEMENT(MPublishEvent##dir##Package);

/*--------------------------------------------------------------------------------
 * M_PUBLISH_DIR
 * Publishing without packaging. Will simply copy an entire directory into
 * the published directory
 *-------------------------------------------------------------------------------*/
#define M_PUBLISH_DIR(dir) \
class MPublishEvent##dir : public MPublishEvent \
{ \
	void	execute(const char* projName) \
	{ \
		MEngine* engine = MEngine::getInstance(); \
		MSystemContext* system = engine->getSystemContext(); \
		char directory[256]; \
		getGlobalFilename(directory, system->getWorkingDirectory(), #dir); \
		char destDirectory[256]; \
		getGlobalFilename(destDirectory, getDataDir(), #dir); \
		copyDirectory(directory, destDirectory); \
	} \
	int		getPriority() { return 5; } \
}; \
M_PUBLISH_EVENT_IMPLEMENT(MPublishEvent##dir); \

M_PUBLISH_PACKAGE_DIR(maps)
M_PUBLISH_PACKAGE_DIR(scripts)
M_PUBLISH_PACKAGE_DIR(sounds)
M_PUBLISH_PACKAGE_DIR(shaders)
M_PUBLISH_PACKAGE_DIR(fonts)
M_PUBLISH_PACKAGE_DIR(levels)
M_PUBLISH_DIR(plugins)


// write and pack binary mesh files
class MPublishEventMeshsPackage : public MPublishEvent
{
	void execute(const char* projName)
	{
		MEngine* engine = MEngine::getInstance();
		MSystemContext* system = engine->getSystemContext();
		MPackageManager* packageManager = engine->getPackageManager();

		char directory[256], localFilename[256];
		getGlobalFilename(directory, system->getWorkingDirectory(), "meshs");
		vector<string> files;
		readDirectory(directory, &files, 1, 1);

		MLevel* currentLevel = engine->getLevel();
		MLevel* tempLevel = new MLevel();
		engine->setLevel(tempLevel);

		MMesh* mesh = MMesh::getNew();
		MArmatureAnim* armAnim = MArmatureAnim::getNew();
		MTexturesAnim* texAnim = MTexturesAnim::getNew();
		MMaterialsAnim* matAnim = MMaterialsAnim::getNew();

		// open package and scan meshes
		MPackage package = openProjectPackage(projName);
		for(int i = 0; i < files.size(); ++i)
		{
			bool binarized = false;
			
			if(strstr(files[i].c_str(), "._bin") != 0)
				continue;
			
			// export bin
			if(strstr(files[i].c_str(), ".mesh") != 0)
			{
				if(engine->getMeshLoader()->loadData(files[i].c_str(), mesh))
					binarized = exportMeshBin((files[i] + "._bin").c_str(), mesh);
			}
			else if (strstr(files[i].c_str(), ".maa") != 0)
			{
				if(engine->getArmatureAnimLoader()->loadData(files[i].c_str(), armAnim))
					binarized = exportArmatureAnimBin((files[i] + "._bin").c_str(), armAnim);
			}
			else if (strstr(files[i].c_str(), ".mma") != 0)
			{
				if(engine->getMaterialsAnimLoader()->loadData(files[i].c_str(), matAnim))
					binarized = exportMaterialsAnimBin((files[i] + "._bin").c_str(), matAnim);
			}
			else if (strstr(files[i].c_str(), ".mta") != 0)
			{
				if(engine->getTexturesAnimLoader()->loadData(files[i].c_str(), texAnim))
					binarized = exportTexturesAnimBin((files[i] + "._bin").c_str(), texAnim);
			}
			else
			{
				// try to export unknow format
				if(engine->getMeshLoader()->loadData(files[i].c_str(), mesh))
				{
					binarized = exportMeshBin((files[i] + "._bin").c_str(), mesh);
					
					// try to export animation
					MArmatureAnimRef * maaRef = mesh->getArmatureAnimRef();
					if(maaRef)
					{
						exportArmatureAnimBin((files[i] + ".maa._bin").c_str(), maaRef->getArmatureAnim());
						getLocalFilename(localFilename, system->getWorkingDirectory(), (files[i] + ".maa").c_str());
						packageManager->addFileToPackage((files[i] + ".maa._bin").c_str(), package, localFilename);
					}
				}
			}

			tempLevel->clear();

			// pack file
			getLocalFilename(localFilename, system->getWorkingDirectory(), files[i].c_str());
			
			if(binarized)
				packageManager->addFileToPackage((files[i] + "._bin").c_str(), package, localFilename); // pack bin file
			else
				packageManager->addFileToPackage(files[i].c_str(), package, localFilename); // pack original file
		}
		packageManager->closePackage(package);

		
		// clear mesh
		mesh->destroy();
		armAnim->destroy();
		texAnim->destroy();
		matAnim->destroy();

		// remove bin
		for(int i = 0; i < files.size(); ++i)
		{
			remove((files[i] + "._bin").c_str());
			remove((files[i] + ".maa._bin").c_str());
		}

		// restore level
		engine->setLevel(currentLevel);
		SAFE_DELETE(tempLevel);
	}
	int getPriority() { return 5; }
};
M_PUBLISH_EVENT_IMPLEMENT(MPublishEventMeshsPackage);


/*--------------------------------------------------------------------------------
 * embedProject
 * Opens the player executable and writes the project data into the
 * static data.
 *-------------------------------------------------------------------------------*/
static void embedProject(const char * src, const char * dest, const char * game, const char * level, const char * renderer)
{
	FILE* fp = 0;
	fp = fopen(src, "rb");
	if(! fp)
		return;

	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	rewind(fp);
	char* buff = new char[size];
	if(size != fread(buff, sizeof(char), size, fp))
	{
		fclose(fp);
		delete [] buff;
		return;
	}
	fclose(fp);

	// look for the embedded data and replace it
	for(char* pChar = buff; pChar != buff+size; ++pChar)
	{
		if(*pChar == '[')
		{
			if(strcmp(pChar, s_embedded_game_name) == 0)
			{
				strcpy(pChar, game);
			}
			else if(strcmp(pChar, s_embedded_level_name) == 0)
			{
				strcpy(pChar, level);
			}
			else if(strcmp(pChar, s_embedded_renderer) == 0)
			{
				strcpy(pChar, renderer);
			}
		}
	}

	fp = fopen(dest, "wb");
	fwrite(buff, sizeof(char), size, fp);
	fclose(fp);
}

static void copyDirFiles(const char * src, const char * dest, const char * filter)
{
	vector<string> files;
	readDirectory(src, &files);
	for(int i = 0; i < files.size(); ++i)
	{
		if(strstr(files[i].c_str(), filter))
		{
			char filename[256];
			getGlobalFilename(filename, dest, files[i].c_str());
			copyFile(files[i].c_str(), filename);
		}
	}
}

/*--------------------------------------------------------------------------------
 * copySysWindows/copySysOSX/copySysLinux
 * Copy Maratis system files to the published directory
 *-------------------------------------------------------------------------------*/
#ifdef WIN32
void copySysWindows(const char* projName)
{
#ifdef _DEBUG
	const char * appName = "MaratisPlayerDebug.exe";
#else
	const char * appName = "MaratisPlayer.exe";
#endif

	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char filename[256];
	getLocalFilename(filename, system->getWorkingDirectory(), projName);

	if(char* ext = strstr(filename, ".mproj"))
	{
		MProject proj;
		if(proj.loadXML(projName))
		{
			strcpy(ext, ".exe");
			char destName[256];
			getGlobalFilename(destName, getPubDir(), filename);

			char level[256];
			getLocalFilename(level, system->getWorkingDirectory(), proj.startLevel.c_str());

			// we need the project "filename" to still be a .mproj for MaratisPlayer to behave
			// correctly
			strcpy(ext, ".mproj");
			embedProject(appName, destName, filename, level, proj.renderer.c_str());

			// find all dynamic libraries
			copyDirFiles(".", getPubDir(), ".dll");
		}
	}
}
#endif

#ifdef __APPLE__
void copySysOSX(const char* projName)
{
#ifdef _DEBUG
	const char * appName = "MaratisPlayerDebug";
#else
	const char * appName = "MaratisPlayer";
#endif

	MWindow * window = MWindow::getInstance();
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char filename[256];
	getLocalFilename(filename, system->getWorkingDirectory(), projName);

	if(char* ext = strstr(filename, ".mproj"))
	{
		MProject proj;
		if(proj.loadXML(projName))
		{
			strcpy(ext, ".app");

			char path[256];
			char srcName[256];
			char destName[256];
			char appPath[256];
			char level[256];

			getLocalFilename(level, system->getWorkingDirectory(), proj.startLevel.c_str());
			getGlobalFilename(appPath, getPubDir(), filename);

			sprintf(path, "../../../%s.app", appName);
			getGlobalFilename(srcName, window->getCurrentDirectory(), path);
			copyDirectory(srcName, appPath);

			strcpy(ext, "");
			sprintf(srcName, "%s/Contents/MacOS/%s", appPath, appName);

			strcpy(ext, ".mproj");
			embedProject(srcName, srcName, filename, level, proj.renderer.c_str());
			chmod(srcName, 0777);

			// we need to put all data in app/Contents/Resources/
			sprintf(destName, "%s/Contents/Resources", appPath);
			createDirectory(destName);
			s_dataDir = destName;
		}
	}
}
#endif

#ifdef linux
void copySysLinux(const char* projName)
{
#ifdef _DEBUG
	const char * appName = "MaratisPlayerDebug";
#else
	const char * appName = "MaratisPlayer";
#endif

	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char filename[256];
	getLocalFilename(filename, system->getWorkingDirectory(), projName);

	if(char* ext = strstr(filename, ".mproj"))
	{
		*ext = 0;

		MProject proj;
		if(proj.loadXML(projName))
		{
			char destName[256];
			getGlobalFilename(destName, getPubDir(), filename);

			char level[256];
			getLocalFilename(level, system->getWorkingDirectory(), proj.startLevel.c_str());

			strcpy(ext, ".mproj");
			embedProject(appName, destName, filename, level, proj.renderer.c_str());
			chmod(destName, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

			// find all dynamic libraries
			copyDirFiles(".", getPubDir(), ".so");
		}
	}
}
#endif


/*--------------------------------------------------------------------------------
 * MPublishEventCopySys
 * TODO: Check if there's a custom "player" executable and load that, rather than
 * MaratisPlayer
 *-------------------------------------------------------------------------------*/
class MPublishEventCopySys : public MPublishEvent
{
	void	execute(const char* projName)
	{

		// for now these are #define'd out. In reality the engine should ship with
		// players for all OS's and then you can choose which OS(/device) you want
		// to publish to
#ifdef WIN32
		copySysWindows(projName);
#elif __APPLE__
		copySysOSX(projName);
#elif linux
		copySysLinux(projName);
#endif
	}

	int		getPriority() { return 1; }
};
M_PUBLISH_EVENT_IMPLEMENT(MPublishEventCopySys);

/*--------------------------------------------------------------------------------
 * MPublishEventCopyGame
 * copy Game plugin, if one exists
 *-------------------------------------------------------------------------------*/
class MPublishEventCopyGame : public MPublishEvent
{
	void	execute(const char* projName)
	{
		MEngine* engine = MEngine::getInstance();
		MSystemContext* system = engine->getSystemContext();

		char filename[256];
		char destFilename[256];

#ifdef WIN32
		const char* pluginName = "Game.dll";
#elif __APPLE__
		const char* pluginName = "Game.dylib";
#elif linux
		const char* pluginName = "Game.so";
#endif

		getGlobalFilename(filename, system->getWorkingDirectory(), pluginName);
		getGlobalFilename(destFilename, getDataDir(), pluginName);

		if(isFileExist(filename))
		{
			copyFile(filename, destFilename);
		}
	}

	int		getPriority() { return 7; }
};
M_PUBLISH_EVENT_IMPLEMENT(MPublishEventCopyGame);

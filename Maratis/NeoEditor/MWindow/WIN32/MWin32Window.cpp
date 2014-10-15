/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWin32Window.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifdef WIN32
#define _WIN32_WINNT 0x0500
#include "../MWindow.h"

#include <MEngine.h>

#if defined(__MINGW32__) || defined(__CYGWIN__)
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <windows.h>
#include <shlobj.h>


const char * MWindow::getTempDirectory(void)
{
	static char tempDirectory[256];
	TCHAR shortPath[MAX_PATH];
	
	GetTempPath(MAX_PATH, shortPath);
	
	LPCTSTR lpszShortPath = shortPath;
	DWORD buffSize = MAX_PATH;
	LPTSTR lpszLongPath = (LPTSTR)malloc(buffSize *sizeof(TCHAR));
	DWORD result = GetLongPathName(lpszShortPath, lpszLongPath, buffSize);
	
	if(result > MAX_PATH)
	{
		delete lpszLongPath;
		buffSize = result;
		lpszLongPath = (LPTSTR)malloc(buffSize*sizeof(TCHAR));
		if(! GetLongPathName(lpszShortPath, lpszLongPath, buffSize))
			return NULL;
	}
	
	strcpy(tempDirectory, lpszLongPath);
	return tempDirectory;
}

const char * MWindow::getCurrentDirectory(void)
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);
	return currentDirectory;
}

void MWindow::setCurrentDirectory(const char * directory)
{
	SetCurrentDirectory(directory);
}

void MWindow::setWorkingDirectory(const char * directory)
{
	strcpy(m_workingDirectory, directory);
}

void MWindow::execute(const char * path, const char * args)
{
	ShellExecute(NULL, "open", path, args, NULL, SW_SHOWNORMAL);
}

void MWindow::executeDetached(const char * path, const char * args, bool killParent)
{
    PROCESS_INFORMATION ProcessInfo;
    STARTUPINFO StartupInfo;

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    if(CreateProcess(path, (LPSTR) args,
        NULL,NULL,FALSE,0,NULL,
        NULL,&StartupInfo,&ProcessInfo))
    {
        if(killParent)
            exit(0);
    }
    else
    {
        MLOG_ERROR("Could not start process!");
    }
}
#endif

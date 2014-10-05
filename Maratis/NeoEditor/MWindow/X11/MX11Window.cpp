/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MX11Window.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2011-2014 Anael Seghezzi <www.maratis3d.com>
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


#ifdef LINUX
#include "../MWindow.h"
#include <MEngine.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char * MWindow::getTempDirectory(void)
{
	static char tempDirectory[8] = "/tmp";
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
	chdir(directory);
}

void MWindow::setWorkingDirectory(const char * directory)
{
	strcpy(m_workingDirectory, directory);
}

void MWindow::execute(const char * path, const char * args)
{
	char dst[512];
	memset(dst, 0, 512);

	strcat(dst, path);
	strcat(dst, " ");
	strcat(dst, args);

	system(dst);
}

static char** vector2array(vector<const char*> vec)
{
    char** arr = new char*[vec.size()];
    for(size_t i = 0; i < vec.size(); i++)
    {
        if(vec[i] == NULL)
        {
            arr[i] = NULL;
            continue;
        }

        arr[i] = new char[strlen(vec[i]) + 1];
        strcpy(arr[i], vec[i]);
    }

    return arr;
}

void MWindow::executeDetached(const char *path, const char *args, bool killParent)
{
    pid_t pid = fork();

    // Child process
    if(pid == 0)
    {
        // Get argument vector
        vector<const char*> vec;
        vec.push_back(path);

        if(args != NULL)
        {
            char* token;
            while((token = strtok((char*) args, " ")) != NULL)
            {
                vec.push_back(token);
            }
        }
        vec.push_back(NULL);
        char** argv = vector2array(vec);

        // Kill parent
        if(killParent)
            kill(getppid(), 9);

        // Execute application!
        execv(path, argv);

        // Should never reach this...
        exit(-1);
    }
    else if(pid < 0)
    {
        MLOG_ERROR("Could not fork process!");
    }
}
#endif

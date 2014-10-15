/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MCocoaWindow.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2010-2014 Anael Seghezzi <www.maratis3d.com>
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


#ifdef __APPLE__
#include "../MWindow.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

const char * MWindow::getTempDirectory(void)
{
	static char tempDirectory[256];
	NSString * temp = NSTemporaryDirectory();
	strcpy(tempDirectory, [temp cStringUsingEncoding:NSUTF8StringEncoding]);
	return tempDirectory;
	
	//return "/tmp"; // linux style
}

const char * MWindow::getCurrentDirectory(void)
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);
	return currentDirectory;
}

void MWindow::setCurrentDirectory(const char * directory)
{
	//chdir(directory);
}

void MWindow::setWorkingDirectory(const char * directory)
{
	strcpy(m_workingDirectory, directory);
}

void MWindow::execute(const char * path, const char * args)
{
	NSString * _path = [NSString stringWithCString:path encoding:NSISOLatin1StringEncoding];
	NSString * _args = [NSString stringWithCString:args encoding:NSISOLatin1StringEncoding];
	
	NSArray * arguments = [NSArray arrayWithObjects: _args, nil, nil];
	
	NSTask *task;
	task = [[NSTask alloc] init];
	[task setLaunchPath:_path];
	[task setArguments:arguments];
	[task launch];
}
#endif

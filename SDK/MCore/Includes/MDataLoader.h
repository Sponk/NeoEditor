/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MDataLoader.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#ifndef _M_DATA_LOADER_H
#define _M_DATA_LOADER_H


// Data load function
class M_CORE_EXPORT MDataLoadFunction
{
public:

	MDataLoadFunction(bool (* functionPtr)(const char * filename, void * data));
	~MDataLoadFunction(void);

	bool (* m_functionPtr)(const char * filename, void * data);
};


// Data loader
class M_CORE_EXPORT MDataLoader
{
private:

	// loaders
	vector <MDataLoadFunction *> m_loaders;

public:

	// constructor / destructor
	MDataLoader(void);
	~MDataLoader(void);

public:

	// clear
	void clear(void);

	// loaders
	void addLoader(bool (* functionPtr)(const char * filename, void * data));

	// data
	bool loadData(const char * filename, void * data);
};

#endif

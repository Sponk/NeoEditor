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


#ifndef __DATA_LOADER_H
#define __DATA_LOADER_H

namespace Neo
{
// Data load function
class NEO_CORE_EXPORT DataLoadFunction
{
public:

	DataLoadFunction(bool (* functionPtr)(const char * filename, void * data));
	~DataLoadFunction(void);

	bool (* m_functionPtr)(const char * filename, void * data);
};


// Data loader
class NEO_CORE_EXPORT DataLoader
{
private:

	// loaders
	vector <DataLoadFunction *> m_loaders;

public:

	// constructor / destructor
	DataLoader(void);
	~DataLoader(void);

public:

	// clear
	void clear(void);

	// loaders
	void addLoader(bool (* functionPtr)(const char * filename, void * data));

	// data
	bool loadData(const char * filename, void * data);
};
}
#endif

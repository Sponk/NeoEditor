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

#include <functional>

namespace Neo
{

// Data loader
template<class T> class NEO_CORE_EXPORT DataLoader
{
private:

	typedef std::function<bool(const char* filename, const char* type, T& data)> save_function_t;
	typedef std::function<bool(const char* filename, T* data)> load_function_t;

	vector<load_function_t> m_loaders;
	vector<save_function_t> m_savers;

public:

	// Data loader
	DataLoader(void)
	{}

	~DataLoader(void)
	{
		clear();
	}

	void clear(void)
	{
		m_loaders.clear();
		m_savers.clear();
	}

	void addLoader(load_function_t function)
	{
		m_loaders.push_back(function);
	}

	void addSaver(save_function_t function)
	{
		m_savers.push_back(function);
	}

	bool loadData(const char * filename, T* data)
	{
		for(load_function_t f : m_loaders)
		{
			if(f(filename, data))
				return true;
		}

		return false;
	}


	bool saveData(const char* filename, const char* type, T& data)
	{
		for(save_function_t f : m_savers)
		{
			if(f(filename, type, data))
				return true;
		}

		return false;
	}
};
}
#endif

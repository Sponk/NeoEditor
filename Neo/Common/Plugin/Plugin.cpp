/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPlugin.cpp
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


#include <NeoEngine.h>
#include "Plugin.h"

using namespace Neo;

typedef void * (*FunctionPtr)();

Plugin::Plugin(void)
{
	m_library = NULL;
}

Plugin::~Plugin(void)
{
	if(m_library)
	{	
#ifdef WIN32
		
		FunctionPtr function = reinterpret_cast<FunctionPtr>(GetProcAddress(m_library, "EndPlugin"));
		if(function)
			 function();
		FreeLibrary(m_library);
		
#else
		
		FunctionPtr function = (FunctionPtr)dlsym(m_library, "EndPlugin");
		if(function)
			function();
		dlclose(m_library);
		
#endif
	}
}

void Plugin::load(const char * filename)
{
#ifdef WIN32
	
    m_library = LoadLibrary(filename);
    if(! m_library)
		return;

    FunctionPtr function = reinterpret_cast<FunctionPtr>(GetProcAddress(m_library, "StartPlugin"));
    if(! function)
		return;

	m_filename = filename;
    function();
	
    // There is no dlopen on Emscripten
#elif !defined(EMSCRIPTEN)
	
	m_library = dlopen(filename, RTLD_LAZY);
	if(! m_library)
		return;
	
	FunctionPtr function = (FunctionPtr)dlsym(m_library, "StartPlugin");
	if(! function)
		return;
	
	m_filename = filename;
    function();

#endif
}

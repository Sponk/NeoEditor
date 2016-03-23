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


#ifndef __RENDERER_CREATOR_H
#define __RENDERER_CREATOR_H

namespace Neo
{
class NEO_ENGINE_EXPORT RendererCreator
{
public:

	RendererCreator(const char * name, Renderer * (*getNewRendererFunctionPointer)(void));
	virtual ~RendererCreator(void){}

private:

	string m_name;
	Renderer * (*m_getNewRenderer)(void);

public:

	// name
	inline void setName(const char * name){ m_name = name; }
	inline const char * getName(void){ return m_name.c_str(); }

	// new renderer
	Renderer * getNewRenderer(void);
};
}
#endif

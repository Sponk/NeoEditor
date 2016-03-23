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


#ifndef __RENDERER_MANAGER_H
#define __RENDERER_MANAGER_H

namespace Neo
{
class NEO_ENGINE_EXPORT RendererManager
{
public:

	// constructors / destructors
	RendererManager(void){}
	virtual ~RendererManager(void);

private:

	vector <RendererCreator *> m_renderers;

public:

	void clear(void);

	// renderers
	void addRenderer(const char * name, Renderer * (*getNewRendererFunctionPointer)(void));
	RendererCreator * getRendererByName(const char * name);

	inline unsigned int getRenderersNumber(void){ return m_renderers.size(); }
	inline RendererCreator * getRendererByIndex(const unsigned int id){ return m_renderers[id]; }
};
}
#endif

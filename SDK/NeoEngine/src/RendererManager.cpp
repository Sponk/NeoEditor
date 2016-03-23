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

using namespace Neo;

RendererManager::~RendererManager(void)
{
	clear();
}

void RendererManager::clear(void)
{
	unsigned int i;
	unsigned int bSize = m_renderers.size();
	for(i=0; i<bSize; i++)
		SAFE_DELETE(m_renderers[i]);

	m_renderers.clear();
}

void RendererManager::addRenderer(const char * name, Renderer * (*getNewRendererFunctionPointer)(void))
{
	RendererCreator * newRCreator = new RendererCreator(name, getNewRendererFunctionPointer);
	m_renderers.push_back(newRCreator);
}

RendererCreator * RendererManager::getRendererByName(const char * name)
{
	unsigned int i;
	unsigned int rSize = getRenderersNumber();
	for(i=0; i<rSize; i++)
	{
		RendererCreator * renderer = getRendererByIndex(i);

		if(! strcmp(name, renderer->getName()))
			return renderer;
	}

	return NULL;
}

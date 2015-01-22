/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MRenderer.h
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


#ifndef _M_RENDERER_H
#define _M_RENDERER_H

namespace Neo
{
// class for creating renderer
class M_ENGINE_EXPORT Renderer
{
public:

	Renderer(void);
	virtual ~Renderer(void);

public:

	// destroy
	virtual void destroy(void) = 0;

	// name
	virtual const char * getName(void) = 0;

	// draw
	virtual void drawScene(Scene * scene, OCamera * camera) = 0;
	
	// extra
	virtual void drawText(OText * text) = 0;
};
}
#endif

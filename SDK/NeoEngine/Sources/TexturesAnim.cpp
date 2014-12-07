/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MTexturesAnim.cpp
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


#include "../Includes/NeoEngine.h"

using namespace Neo;

TexturesAnim::TexturesAnim(void):
m_texturesAnimNumber(0),
m_texturesAnim(NULL)
{}

TexturesAnim::~TexturesAnim(void)
{
	clearTexturesAnim();
}

TexturesAnim * TexturesAnim::getNew(void)
{
	return new TexturesAnim();
}

void TexturesAnim::destroy(void)
{
	delete this;
}

TextureAnim * TexturesAnim::allocTexturesAnim(unsigned int size)
{
	clearTexturesAnim();
	if(size == 0)
		return NULL;

	m_texturesAnimNumber = size;
	m_texturesAnim = new TextureAnim[size];
	return m_texturesAnim;
}

void TexturesAnim::clearTexturesAnim(void)
{
	SAFE_DELETE_ARRAY(m_texturesAnim);
	m_texturesAnimNumber = 0;
}

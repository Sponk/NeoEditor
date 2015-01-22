/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MTextureAnim.h
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


#ifndef _M_TEXTURE_ANIM
#define _M_TEXTURE_ANIM

namespace Neo
{
class M_ENGINE_EXPORT TextureAnim
{
public:

	TextureAnim(void);
	~TextureAnim(void);

private:

	unsigned int m_translateKeysNumber;
	unsigned int m_scaleKeysNumber;
	unsigned int m_rotationKeysNumber;

	Key * m_translateKeys;
	Key * m_scaleKeys;
	Key * m_rotationKeys;

public:

	Key * allocTranslateKeys(unsigned int size);
	Key * allocScaleKeys(unsigned int size);
	Key * allocRotationKeys(unsigned int size);

	void clearTranslateKeys(void);
	void clearScaleKeys(void);
	void clearRotationKeys(void);

	inline unsigned int getTranslateKeysNumber(void){ return m_translateKeysNumber; }
	inline unsigned int getScaleKeysNumber(void){ return m_scaleKeysNumber; }
	inline unsigned int getRotationKeysNumber(void){ return m_rotationKeysNumber; }

	inline Key * getTranslateKeys(void){ return m_translateKeys; }
	inline Key * getScaleKeys(void){ return m_scaleKeys; }
	inline Key * getRotationKeys(void){ return m_rotationKeys; }
};
}
#endif

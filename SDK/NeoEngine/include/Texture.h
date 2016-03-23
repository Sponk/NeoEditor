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


#ifndef __TEXTURE_H
#define __TEXTURE_H

namespace Neo
{
class NEO_ENGINE_EXPORT Texture
{
public:

	// constructor
	Texture(TextureRef * texRef):
		m_texRef(texRef),
		m_genMode(TEX_GEN_NONE),
		m_texTranslate(0, 0),
		m_texScale(1, 1),
		m_texRotate(0)
		{
			m_wrapMode[0] = WRAP_REPEAT;
			m_wrapMode[1] = WRAP_REPEAT;
		}

	// copy constructor
	Texture(const Texture & texture):
		m_texRef(texture.m_texRef),
		m_genMode(texture.m_genMode),
		m_texTranslate(texture.m_texTranslate),
		m_texScale(texture.m_texScale),
		m_texRotate(texture.m_texRotate)
		{
			m_wrapMode[0] = texture.m_wrapMode[0];
			m_wrapMode[1] = texture.m_wrapMode[1];
		}

private:

	// texture ref
	TextureRef * m_texRef;

	// gen mode
	TEX_GEN_MODES m_genMode;

	// wrap mode
	WRAP_MODES m_wrapMode[2];

	// texture matrix
	Vector2 m_texTranslate;
	Vector2 m_texScale;
	float m_texRotate;

public:

	// texture ref
	void setTextureRef(TextureRef * texRef){ m_texRef = texRef; }
	TextureRef * getTextureRef(void){ return m_texRef; }

	// gen mode
	void setGenMode(TEX_GEN_MODES genMode){ m_genMode = genMode; }
	TEX_GEN_MODES getGenMode(void){ return m_genMode; }

	// wrap mode
	void setUWrapMode(WRAP_MODES wrapMode){ m_wrapMode[0] = wrapMode; }
	void setVWrapMode(WRAP_MODES wrapMode){ m_wrapMode[1] = wrapMode; }
	WRAP_MODES getUWrapMode(void){ return m_wrapMode[0]; }
	WRAP_MODES getVWrapMode(void){ return m_wrapMode[1]; }

	// texture matrix
	inline void setTexTranslate(const Vector2 & texTranslate){ m_texTranslate = texTranslate; }
	inline void setTexScale(const Vector2 & texScale){ m_texScale = texScale; }
	inline void setTexRotate(float texRotate){ m_texRotate = texRotate; }

	inline Vector2 getTexTranslate(void) const { return m_texTranslate; }
	inline Vector2 getTexScale(void) const { return m_texScale; }
	inline float getTexRotate(void){ return m_texRotate; }
};
}
#endif

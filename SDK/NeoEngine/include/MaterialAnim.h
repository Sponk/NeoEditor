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


#ifndef __MATERIAL_ANIM
#define __MATERIAL_ANIM

namespace Neo
{
class NEO_ENGINE_EXPORT MaterialAnim
{
public:

	MaterialAnim(void);
	~MaterialAnim(void);

private:

	unsigned int m_opacityKeysNumber;
	unsigned int m_shininessKeysNumber;
	unsigned int m_customValueKeysNumber;
	unsigned int m_diffuseKeysNumber;
	unsigned int m_specularKeysNumber;
	unsigned int m_emitKeysNumber;
	unsigned int m_customColorKeysNumber;

	Key * m_opacityKeys;
	Key * m_shininessKeys;
	Key * m_customValueKeys;
	Key * m_diffuseKeys;
	Key * m_specularKeys;
	Key * m_emitKeys;
	Key * m_customColorKeys;

public:

	Key * allocOpacityKeys(unsigned int size);
	Key * allocShininessKeys(unsigned int size);
	Key * allocCustomValueKeys(unsigned int size);
	Key * allocDiffuseKeys(unsigned int size);
	Key * allocSpecularKeys(unsigned int size);
	Key * allocEmitKeys(unsigned int size);
	Key * allocCustomColorKeys(unsigned int size);

	void clearOpacityKeys(void);
	void clearShininessKeys(void);
	void clearCustomValueKeys(void);
	void clearDiffuseKeys(void);
	void clearSpecularKeys(void);
	void clearEmitKeys(void);
	void clearCustomColorKeys(void);

	inline unsigned int getOpacityKeysNumber(void){ return m_opacityKeysNumber; }
	inline unsigned int getShininessKeysNumber(void){ return m_shininessKeysNumber; }
	inline unsigned int getCustomValueKeysNumber(void){ return m_customValueKeysNumber; }
	inline unsigned int getDiffuseKeysNumber(void){ return m_diffuseKeysNumber; }
	inline unsigned int getSpecularKeysNumber(void){ return m_specularKeysNumber; }
	inline unsigned int getEmitKeysNumber(void){ return m_emitKeysNumber; }
	inline unsigned int getCustomColorKeysNumber(void){ return m_customColorKeysNumber; }

	inline Key * getOpacityKeys(void){ return m_opacityKeys; }
	inline Key * getShininessKeys(void){ return m_shininessKeys; }
	inline Key * getCustomValueKeys(void){ return m_customValueKeys; }
	inline Key * getDiffuseKeys(void){ return m_diffuseKeys; }
	inline Key * getSpecularKeys(void){ return m_specularKeys; }
	inline Key * getEmitKeys(void){ return m_emitKeys; }
	inline Key * getCustomColorKeys(void){ return m_customColorKeys; }
};
}
#endif

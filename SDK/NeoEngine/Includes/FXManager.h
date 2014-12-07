/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MFXManager.h
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


#ifndef _M_FX_MANAGER_H
#define _M_FX_MANAGER_H

namespace Neo
{
// FX ref
class M_ENGINE_EXPORT FXRef
{
public:

	FXRef(unsigned int FXId, ShaderRef * vertexShaderRef, ShaderRef * pixelShaderRef):
		m_FXId(FXId),
		m_vertexShaderRef(vertexShaderRef),
		m_pixelShaderRef(pixelShaderRef)
	{}

	~FXRef(void);

private:

	unsigned int m_FXId;
	ShaderRef * m_vertexShaderRef;
	ShaderRef * m_pixelShaderRef;

public:

	inline unsigned int getFXId(void){ return m_FXId; }
	inline ShaderRef * getVertexShaderRef(void){ return m_vertexShaderRef; }
	inline ShaderRef * getPixelShaderRef(void){ return m_pixelShaderRef; }
};


// FX manager
class M_ENGINE_EXPORT FXManager
{
private:

	// refs
	vector <FXRef *> m_FXRefs;

public:

	// destructor
	virtual ~FXManager(void);

public:

	// clear
	virtual void clear(void);

	// fx refs
	FXRef * addFXRef(unsigned int FXId, ShaderRef * vertexShaderRef, ShaderRef * pixelShaderRef);
	inline FXRef * getFXRef(unsigned int id){ return m_FXRefs[id]; }
	inline unsigned int getFXRefsNumber(void){ return m_FXRefs.size(); }
};
}
#endif

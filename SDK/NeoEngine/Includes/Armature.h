/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MArmature.h
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


#ifndef _M_ARMATURE_H
#define _M_ARMATURE_H

namespace Neo
{
class M_ENGINE_EXPORT Armature
{
private:

	// bones
	unsigned int m_bonesNumber;
	OBone ** m_bones;

public:

	// constructors / destructor
	Armature(void);
	virtual ~Armature(void);

public:

	// bones
	void clearBones(void);
	void allocBones(unsigned int size);
	bool getBoneId(const char * boneName, unsigned int * bId);
	OBone * addNewBone(void);
	OBone * getBoneByName(const char * name);
	inline unsigned int	getBonesNumber(void){ return m_bonesNumber; }
	inline OBone *	getBone(unsigned int bId){ return m_bones[bId]; }

	// construct bones inverse pose matrix
	void constructBonesInversePoseMatrix(void);

	// updat bones skin matrix
	void updateBonesSkinMatrix(void);

	// linking
	void processBonesLinking(void);
};
}
#endif

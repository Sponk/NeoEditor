//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __ARMATURE_H
#define __ARMATURE_H

namespace Neo
{
class NEO_ENGINE_EXPORT Armature
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

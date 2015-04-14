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


#ifndef __OBONE_H
#define __OBONE_H

namespace Neo
{
class NEO_ENGINE_EXPORT OBone : public Object3d
{
public:

	// constructors
	OBone(void);

	// destructor
	virtual ~OBone(void);

	// copy constructor
	OBone(const OBone & bone);

	// inverse pose matrix
	Matrix4x4 m_inversePoseMatrix;

	// skin matrix
	Matrix4x4 m_skinMatrix;

public:

	// type
	int getType(void){ return M_OBJECT3D_BONE; }

	// childs linking
	void processChildrenLinking(void);

	// inverse pose matrix
	Matrix4x4 * getInversePoseMatrix(void){ return &m_inversePoseMatrix; }

	// skin matrix
	Matrix4x4 * getSkinMatrix(void){ return &m_skinMatrix; }
};
}
#endif

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


#ifndef __BOX3D_H
#define __BOX3D_H

namespace Neo
{
class NEO_ENGINE_EXPORT Box3d
{
public:

	Box3d(void);
	Box3d(const Box3d & box);
	Box3d(const Vector3 & _min, const Vector3 & _max);

	Vector3 min;
	Vector3 max;

	void initFromPoints(const Vector3 * points, unsigned int pointsNumber);
	bool isInCollisionWith(const Box3d & box);
};
}

#endif

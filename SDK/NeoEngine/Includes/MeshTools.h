/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMeshTools.h
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


#ifndef _M_MESH_TOOLS_H
#define _M_MESH_TOOLS_H

namespace Neo
{

M_ENGINE_EXPORT bool animateFloat(Key * keys, unsigned int keysNumber, float t, float * value);
M_ENGINE_EXPORT bool animateVector2(Key * keys, unsigned int keysNumber, float t, Vector2 * vector2);
M_ENGINE_EXPORT bool animateVector3(Key * keys, unsigned int keysNumber, float t, Vector3 * vector3);
M_ENGINE_EXPORT bool animateQuaternion(Key * keys, unsigned int keysNumber, float t, Quaternion * quaternion);

M_ENGINE_EXPORT void animateArmature(
	Armature * armature,
	ArmatureAnim * armatureAnim,
	float t
	);

M_ENGINE_EXPORT void animateTextures(
	Mesh * mesh,
	TexturesAnim * texturesAnim,
	float t
	);

M_ENGINE_EXPORT void animateMaterials(
	Mesh * mesh,
	MaterialsAnim * materialsAnim,
	float t
	);

M_ENGINE_EXPORT void computeSkinning(
	Armature * armature,
	SkinData * skinData,
	const Vector3 * baseVertices,
	const Vector3 * baseNormals,
	const Vector3 * baseTangents,
	Vector3 * vertices,
	Vector3 * normals,
	Vector3 * tangents
	);

M_ENGINE_EXPORT bool isRaytraced(
	const Vector3 & origin,
	const Vector3 & dest, 
	const void * indices, 
	VAR_TYPES indicesType,
	const Vector3 * vertices, 
	unsigned int size
	);

M_ENGINE_EXPORT bool getNearestRaytracedPosition(
	const Vector3 & origin, 
	const Vector3 & dest, 
	const void * indices, 
	VAR_TYPES indicesType,
	const Vector3 * vertices, 
	unsigned int size, 
	Vector3 * intersection,
	bool invertNormal = false
	);
}
#endif

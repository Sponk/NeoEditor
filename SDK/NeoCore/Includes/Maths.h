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


#ifndef __MATHS_H
#define __MATHS_H

#include <math.h>


// PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Degree/Radian conversions
#define DEG_TO_RAD ((M_PI * 2) / 360.0)
#define RAD_TO_DEG (1.0 / DEG_TO_RAD)

// MIN
#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

// MAX
#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

// CLAMP
#ifndef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

// ABS
#ifndef ABS
#define ABS(a)  (((a) < 0) ? -(a) : (a))
#endif

// Power of two
#define isPowerOfTwo(x)	(((x&(x - 1)) == 0) && (x != 0))

namespace Neo
{

NEO_CORE_EXPORT unsigned int getNextPowerOfTwo(unsigned int x);

// loop
NEO_CORE_EXPORT float loopFloat(float val, float min, float max);


// structs
struct NEO_CORE_EXPORT Range
{
	int start;
	int end;
};

// classes
class Vector2;
class Vector3;
class Vector4;
class Color;
class Matrix4x4;
class Quaternion;


// functions
NEO_CORE_EXPORT void sortFloatList(int indexList[], float floatList[], int start, int end);
NEO_CORE_EXPORT void sortFloatList(float floatList[], int start, int end);
NEO_CORE_EXPORT bool isBoxToBoxCollision(const Vector3 & minA, const Vector3 & maxA, const Vector3 & minB, const Vector3 & maxB);
NEO_CORE_EXPORT bool isBoxToBox2dCollision(const Vector2 & minA, const Vector2 & maxA, const Vector2 & minB, const Vector2 & maxB);
NEO_CORE_EXPORT bool isPointInBox(const Vector3 & point, const Vector3 & min, const Vector3 & max);
NEO_CORE_EXPORT bool isPointInBox2d(const Vector2 & point, const Vector2 & min, const Vector2 & max);
NEO_CORE_EXPORT bool isEdgeToBoxCollision(const Vector3 & origin, const Vector3 & dest, const Vector3 & min, const Vector3 & max);
NEO_CORE_EXPORT bool isEdgeToEdge2dIntersection(const Vector2 & A, const Vector2 & B, const Vector2 & C, const Vector2 & D, Vector2 * I);
NEO_CORE_EXPORT bool isPointInTriangle(const Vector3 & point, const Vector3 & a, const Vector3 & b, const Vector3 & c, const Vector3 & normal);
NEO_CORE_EXPORT bool isLineCircleIntersection(const Vector2 & origin, const Vector2 & dest, const Vector2 & circleCenter, float circleRadius);
NEO_CORE_EXPORT bool isRaySphereIntersection(const Vector3 & origin, const Vector3 & direction, const Vector3 & sphereCenter, float sphereRadius, Vector3 * point);
NEO_CORE_EXPORT bool isRayPlaneIntersection(const Vector3 & origin, const Vector3 & direction, const Vector3 & planePoint, const Vector3 & planeNormal, Vector3 * point);
NEO_CORE_EXPORT bool isEdgePlaneIntersection(const Vector3 & origin, const Vector3 & dest, const Vector3 & planePoint, const Vector3 & normal, Vector3 * point);
NEO_CORE_EXPORT bool isEdgeTriangleIntersection(const Vector3 & origin, const Vector3 & dest, const Vector3 & a, const Vector3 & b, const Vector3 & c, const Vector3 & normal, Vector3 * point);

NEO_CORE_EXPORT Vector3 getTriangleNormal(const Vector3 & a, const Vector3 & b, const Vector3 & c);

NEO_CORE_EXPORT void simplifyDP(float tol, Vector3 * v, int j, int k, int * mk);

NEO_CORE_EXPORT float linearInterpolation(float y1, float y2, float mu);
NEO_CORE_EXPORT float cubicInterpolation(float y0, float y1, float y2, float y3, float mu);
NEO_CORE_EXPORT float CatmullRomInterpolation(float y0, float y1, float y2, float y3, float mu);
NEO_CORE_EXPORT float HermiteInterpolation(float y0, float y1, float y2, float y3, float mu, float tension, float bias, bool begin = false, bool end = false);

// color convertions
NEO_CORE_EXPORT float HueToRGB(float v1, float v2, float vH);
NEO_CORE_EXPORT Vector3 RGBToHSV(Vector3 rgbColor);
NEO_CORE_EXPORT Vector3 HSVToRGB(Vector3 HSVColor);
NEO_CORE_EXPORT Vector3 RGBToHSL(Vector3 rgbColor);
NEO_CORE_EXPORT Vector3 HSLToRGB(Vector3 hslColor);
}

// include classes
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Color.h>
#include <Matrix4x4.h>
#include <Quaternion.h>

#endif

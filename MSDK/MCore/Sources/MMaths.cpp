/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMaths.cpp
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


#include "../Includes/MCore.h"


void sortFloatList(int indexList[], float floatList[], int start, int end)
{
	int i = start;
	int j = end;
	float middle = floatList[indexList[(start + end)/2]];
	
	while(i <= j)
	{
		while(floatList[indexList[i]] > middle) i++;
		while(floatList[indexList[j]] < middle) j--;
		
		if(i <= j)
		{
			int temp = indexList[i];
			indexList[i] = indexList[j];
			indexList[j] = temp;
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatList(indexList, floatList, i, end);
	if(start < j)
		sortFloatList(indexList, floatList, start, j);
}

void sortFloatList(float floatList[], int start, int end)
{
	int i = start;
	int j = end;
	float middle = floatList[(start + end)/2];
	
	while(i <= j)
	{
		while(floatList[i] > middle) i++;
		while(floatList[j] < middle) j--;
		
		if(i <= j)
		{
			float temp = floatList[i];
			floatList[i] = floatList[j];
			floatList[j] = temp;
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatList(floatList, i, end);
	if(start < j)
		sortFloatList(floatList, start, j);
}

float loopFloat(float val, float min, float max)
{
	if(val >= min && val < max)
		return val;
	
	float d = max - min;
	if(d <= 0)
		return min;
	
	float v = (val - min) / d;
	v = v - (float)((int)v);
	
	if(v < 0)
		return max + (d*v);
	else
		return min + (d*v);
}

unsigned int getNextPowerOfTwo(unsigned int x)
{
	if(x == 0)
		return 0;

	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;

	return x;
}

bool isEdgeToEdge2dIntersection(const MVector2 & A, const MVector2 & B, const MVector2 & C, const MVector2 & D, MVector2 * I)
{
	MVector2 DP, QA, QB;
	float d, la, lb;

	DP.x = C.x - A.x ; DP.y = C.y - A.y;
	QA.x = B.x - A.x ; QA.y = B.y - A.y;
	QB.x = D.x - C.x ; QB.y = D.y - C.y;

	d  =   QA.y * QB.x - QB.y * QA.x;
	la = ( QB.x * DP.y - QB.y * DP.x ) / d;

	if(la < 0)
		return false;

	if(la > 1)
		return false;

	lb = ( QA.x * DP.y - QA.y * DP.x ) / d;

	if(lb < 0)
		return false;

	if(lb > 1)
		return false;

	I->x = A.x + la * QA.x;
	I->y = A.y + la * QA.y;

	return true;
}

bool isLineCircleIntersection(const MVector2 & origin, const MVector2 & dest, const MVector2 & circleCenter, float circleRadius)
{
	float rr = circleRadius*circleRadius;

	if((origin - circleCenter).getSquaredLength() < rr)
		return true;

	if((dest - circleCenter).getSquaredLength() < rr)
		return true;

	MVector2 vec = dest - origin;
	MVector2 N = MVector2(-vec.y, vec.x).getNormalized();

	float d = N.dotProduct(circleCenter - origin);

	if(fabs(d) >= circleRadius)
		return false;

	MVector2 I = circleCenter - (N * d);

	if(fabs(vec.x) > fabs(vec.y))
	{
		if(((I.x > origin.x) && (I.x < dest.x)) ||
		   ((I.x < origin.x) && (I.x > dest.x)))
		   return true;

		return false;
	}

	if(((I.y > origin.y) && (I.y < dest.y)) ||
	   ((I.y < origin.y) && (I.y > dest.y)))
	   return true;

	return false;
}

bool isBoxToBoxCollision(const MVector3 & minA, const MVector3 & maxA, const MVector3 & minB, const MVector3 & maxB)
{
	// X
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;

	// Y
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;

	// Z
	if(minA.z > maxB.z) return false;
	if(maxA.z < minB.z) return false;

	return true;
}

bool isBoxToBox2dCollision(const MVector2 & minA, const MVector2 & maxA, const MVector2 & minB, const MVector2 & maxB)
{
	// X
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;

	// Y
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;

	return true;
}

bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max)
{
	if(point.x >= (min.x-0.01f) && point.x <= (max.x+0.01f) &&
	   point.y >= (min.y-0.01f) && point.y <= (max.y+0.01f) &&
	   point.z >= (min.z-0.01f) && point.z <= (max.z+0.01f))
	   return true;

	return false;
}

bool isPointInBox2d(const MVector2 & point, const MVector2 & min, const MVector2 & max)
{
	if(point.x > max.x)
		return false;

	if(point.x < min.x)
		return false;

	if(point.y > max.y)
		return false;

	if(point.y < min.y)
		return false;

	return true;
}

bool isEdgeToBoxCollision(const MVector3 & origin, const MVector3 & dest, const MVector3 & min, const MVector3 & max)
{
	MVector3 nrm[6];
	MVector3 pts[6];
	MVector3 pt;

	nrm[0].set( 1, 0, 0); pts[0] = max;
	nrm[1].set(-1, 0, 0); pts[1] = min;
	nrm[2].set( 0, 1, 0); pts[2] = max;
	nrm[3].set( 0,-1, 0); pts[3] = min;
	nrm[4].set( 0, 0, 1); pts[4] = max;
	nrm[5].set( 0, 0,-1); pts[5] = min;

	if(isPointInBox(origin, min, max) || isPointInBox(dest, min, max))
		return true;

	for(int i=0; i<6; i++)
	{
		if(isEdgePlaneIntersection(origin, dest, pts[i], nrm[i], &pt))
		{
			if(isPointInBox(pt, min, max))
				return true;
		}
	}

	return false;
}

bool isEdgePlaneIntersection(const MVector3 & origin, const MVector3 & dest, const MVector3 & planePoint, const MVector3 & normal, MVector3 * point)
{
	MVector3 vec_tmp[2], vec[2];
	float pts_dst[2], val_tmp[2];

	vec[0].x = origin.x - planePoint.x;
	vec[0].y = origin.y - planePoint.y;
	vec[0].z = origin.z - planePoint.z;

	vec[1].x = dest.x - planePoint.x;
	vec[1].y = dest.y - planePoint.y;
	vec[1].z = dest.z - planePoint.z;

	// points distance
	pts_dst[0] = vec[0].dotProduct(normal);

	if(pts_dst[0] > 0)
	{
		pts_dst[1] = vec[1].dotProduct(normal);

		if(pts_dst[1] < 0)
		{
			if(! point)
				return true;

			vec_tmp[0].x = origin.x - dest.x;
			vec_tmp[0].y = origin.y - dest.y;
			vec_tmp[0].z = origin.z - dest.z;

			val_tmp[0] = vec_tmp[0].dotProduct(normal);
			val_tmp[1] = pts_dst[0] / val_tmp[0];

			point->x = origin.x - vec_tmp[0].x * val_tmp[1];
			point->y = origin.y - vec_tmp[0].y * val_tmp[1];
			point->z = origin.z - vec_tmp[0].z * val_tmp[1];

			return true;
		}
	}

	return false;
}

bool isRaySphereIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & sphereCenter, float sphereRadius, MVector3 * point)
{
	MVector3 vec = origin - sphereCenter;
	float b = direction.dotProduct(vec);
	float c = vec.getSquaredLength() - (sphereRadius * sphereRadius);
	float d = (b * b) - c;

	if(d < 0)
		return false;

	float distance = -b - sqrtf(d);

	point->x = (origin.x + (distance * direction.x));
	point->y = (origin.y + (distance * direction.y));
	point->z = (origin.z + (distance * direction.z));

	return true;
}

bool isRayPlaneIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & planePoint, const MVector3 & planeNormal, MVector3 * point)
{
	float constant = - planeNormal.dotProduct(planePoint);

	float normalDotDir = planeNormal.dotProduct(direction);
	float planeDistance = planeNormal.dotProduct(origin) + constant;

	float t = - planeDistance / normalDotDir;

	point->x = (origin.x + (t * direction.x));
	point->y = (origin.y + (t * direction.y));
	point->z = (origin.z + (t * direction.z));

	return true;
}

bool isPointInTriangle(const MVector3 & point, const MVector3 & a, const MVector3 & b, const MVector3 & c, const MVector3 & normal)
{
	MVector3 nrm = getTriangleNormal(point, a, b);

	if(nrm.dotProduct(normal) < 0)
		return false;

	nrm = getTriangleNormal(point, b, c);
	if(nrm.dotProduct(normal) < 0)
		return false;

	nrm = getTriangleNormal(point, c, a);
	if(nrm.dotProduct(normal) < 0)
		return false;

	return true;
}

bool isEdgeTriangleIntersection(const MVector3 & origin, const MVector3 & dest, const MVector3 & a, const MVector3 & b, const MVector3 & c, const MVector3 & normal, MVector3 * point)
{
	MVector3 iPoint;

	if(isEdgePlaneIntersection(origin, dest, a, normal, &iPoint))
	{
		if(isPointInTriangle(iPoint, a, b, c, normal))
		{
			if(! point)
				return true;

			point->x = iPoint.x;
			point->y = iPoint.y;
			point->z = iPoint.z;
			return true;
		}
	}

	return false;
}

MVector3 getTriangleNormal(const MVector3 & a, const MVector3 & b, const MVector3 & c)
{
	MVector3 vec0 = a - b;
	MVector3 vec1 = a - c;

	return vec0.crossProduct(vec1).getNormalized();
}

float linearInterpolation(float y1, float y2, float mu)
{
	return (y1 + (y2 - y1)*mu);
}

float cubicInterpolation(float y0, float y1, float y2, float y3, float mu)
{
	float a0,a1,a2,a3,mu2;
	
	mu2 = mu*mu;
	
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;
	
	return (a0*mu*mu2+a1*mu2+a2*mu+a3);
}

float CatmullRomInterpolation(float y0, float y1, float y2, float y3, float mu)
{
	float a0,a1,a2,a3,mu2;
	
	mu2 = mu*mu;
	
	a0 = -0.5f*y0 + 1.5f*y1 - 1.5f*y2 + 0.5f*y3;
	a1 = y0 - 2.5f*y1 + 2.0f*y2 - 0.5f*y3;
	a2 = -0.5f*y0 + 0.5f*y2;
	a3 = y1;
	
	return (a0*mu*mu2+a1*mu2+a2*mu+a3);
}

float HermiteInterpolation(float y0, float y1, float y2, float y3, float mu, float tension, float bias, bool begin, bool end)
{
	float m0,m1,mu2,mu3;
	float a0,a1,a2,a3;
	
	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1-y0)*(1+bias)*(1-tension)*0.5f;
	m1 = (y3-y2)*(1-bias)*(1-tension)*0.5f;
	
	if(!begin)
		m0 += (y2-y1)*(1-bias)*(1-tension)*0.5f;
		
	if(!end)
		m1 += (y2-y1)*(1+bias)*(1-tension)*0.5f;
	
	a0 =  2*mu3 - 3*mu2 + 1;
	a1 =    mu3 - 2*mu2 + mu;
	a2 =    mu3 -   mu2;
	a3 = -2*mu3 + 3*mu2;
	
	return(a0*y1+a1*m0+a2*m1+a3*y2);
}

MVector3 RGBToHSV(MVector3 rgbColor)
{
	float r = rgbColor.x;
	float g = rgbColor.y;
	float b = rgbColor.z;
	
	float h = 0;
	float s = 0;
	float v = 0;
	float min, max, delta;
	
	min = r;
	if(g < min) min = g;
	if(b < min) min = b;
	
	max = r;
	if(g > max) max = g;
	if(b > max) max = b;
	
	v = max;
	
	delta = max - min;
	
	if(delta == 0)
		return MVector3(h, s, v);
	
	if(max != 0)
		s = delta/max;
	else
		return MVector3(h, s, v);
	
	if(r == max)
		h = (g - b) / delta;
	else if(g == max)
		h = 2 + (b - r) / delta;
	else
		h = 4 + (r - g) / delta;
	
	h *= 60;
	if(h < 0)
		h += 360;
	
	return MVector3(h, s, v);
}

MVector3 HSVToRGB(MVector3 HSVColor)
{
	int i;
	float r, g, b;
	float f, p, q, t;
	
	float h = HSVColor.x;
	float s = HSVColor.y;
	float v = HSVColor.z;
	
	if(s == 0)
	{
		r = g = b = v;
		return MVector3(r, g, b);
	}
	
	h /= 60;
	i = (int)floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	
	switch(i)
	{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:
			r = v;
			g = p;
			b = q;
			break;
	}
	
	return MVector3(r, g, b);
}

MVector3 RGBToHSL(MVector3 rgbColor)
{
	float varMin = rgbColor.x;
	if(rgbColor.y < varMin)
		varMin = rgbColor.y;
	if(rgbColor.z < varMin)
		varMin = rgbColor.z;
	
	float varMax = rgbColor.x;
	if(rgbColor.y > varMax)
		varMax = rgbColor.y;
	if(rgbColor.z > varMax)
		varMax = rgbColor.z;
	
	float delMax = varMax - varMin;
	float L = (varMax + varMin) / 2;
	float H = 0, S = 0;
	
	if(delMax == 0)
		return MVector3(H, S, L);
	
	if(L < 0.5f)
		S = delMax / (varMax + varMin);
	else
		S = delMax / (2 - varMax - varMin);
	
	float delR = (((varMax - rgbColor.x) / 6.0f) + (delMax / 2.0f) ) / delMax;
	float delG = (((varMax - rgbColor.y) / 6.0f) + (delMax / 2.0f) ) / delMax;
	float delB = (((varMax - rgbColor.z) / 6.0f) + (delMax / 2.0f) ) / delMax;
	
	if(rgbColor.x == varMax)
		H = delB - delG;
	else if(rgbColor.y == varMax)
		H = (1.0f / 3.0f) + delR - delB;
	else if(rgbColor.z == varMax)
		H = (2.0f / 3.0f) + delG - delR;
	
	if(H < 0)
		H += 1;
	if(H > 1)
		H -= 1;
	
	return MVector3(H, S, L);
}

float HueToRGB(float v1, float v2, float vH)
{
	if(vH < 0)
		vH += 1;
	
	if(vH > 1)
		vH -= 1;
	
	if((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);
	
	if((2 * vH) < 1)
		return v2;
	
	if((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6);
	
	return v1;
}

MVector3 HSLToRGB(MVector3 hslColor)
{
	float H = hslColor.x;
	float S = hslColor.y;
	float L = hslColor.z;
	
	if(S == 0)
		return MVector3(L, L, L);
	
	float var1, var2;
	
	if(L < 0.5f)
		var2 = L * (1 + S);
	else
		var2 = (L + S) - (S * L);
	
	var1 = 2 * L - var2;
	
	float R = HueToRGB(var1, var2, H + (1.0f / 3.0f));
	float G = HueToRGB(var1, var2, H);
	float B = HueToRGB(var1, var2, H - (1.0f / 3.0f));
	
	return MVector3(R, G, B);
}

// simplifyDP():

// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// Modified by Anaël Seghezzi to use MVector3

//  This is the Douglas-Peucker recursive simplification routine
//  It just marks vertices that are part of the simplified polyline
//  for approximating the polyline subchain v[j] to v[k].
//    Input:  tol = approximation tolerance
//            v[] = polyline array of vertex points
//            j,k = indices for the subchain v[j] to v[k]
//    Output: mk[] = array of markers matching vertex array v[]

void simplifyDP(float tol, MVector3 * v, int j, int k, int * mk)
{
    if(k <= j+1) // there is nothing to simplify
        return;
	
    // check for adequate approximation by segment S from v[j] to v[k]
    int			maxi = j;					// index of vertex farthest from S
    float		maxd2 = 0;					// distance squared of farthest vertex
    float		tol2 = tol * tol;			// tolerance squared
    MVector3	S[2] = {v[j], v[k]};		// segment from v[j] to v[k]
    MVector3	u = S[1] - S[0];			// segment direction vector
    float		cu = u.getSquaredLength();	// segment length squared
	
    // test each vertex v[i] for max distance from S
    // compute using the Feb 2001 Algorithm's dist_Point_to_Segment()
    // Note: this works in any dimension (2D, 3D, ...)
    MVector3 w;
    MVector3 Pb;               // base of perpendicular from v[i] to S
    float  b, cw, dv2;        // dv2 = distance v[i] to S squared
	
    for(int i=j+1; i<k; i++)
    {
        // compute distance squared
        w = v[i] - S[0];
        cw = w.dotProduct(u);
        if(cw <= 0)
            dv2 = (v[i] - S[0]).getSquaredLength();
        else if ( cu <= cw )
            dv2 = (v[i] - S[1]).getSquaredLength();
        else {
            b = cw / cu;
            Pb = S[0] + b * u;
            dv2 = (v[i] - Pb).getSquaredLength();
        }
		
        // test with current max distance squared
        if (dv2 <= maxd2)
            continue;
		
        // v[i] is a new max vertex
        maxi = i;
        maxd2 = dv2;
    }
	
    if(maxd2 > tol2) // error is worse than the tolerance
    {
        // split the polyline at the farthest vertex from S
        mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
		
        // recursively simplify the two subpolylines at v[maxi]
        simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
        simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
    }
	
    // else the approximation is OK, so ignore intermediate vertices
    return;
}
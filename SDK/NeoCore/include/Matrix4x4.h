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


#ifndef __MATRIX4X4_H
#define __MATRIX4X4_H

namespace Neo
{
class NEO_CORE_EXPORT Matrix4x4
{
public:

	float entries[16];

public:

	Matrix4x4(void){	
		loadIdentity();
	}

	Matrix4x4(	float e0,	float e1,	float e2,	float e3,
				float e4,	float e5,	float e6,	float e7,
				float e8,	float e9,	float e10,	float e11,
				float e12,	float e13,	float e14,	float e15);

	Matrix4x4(const float * value);
	Matrix4x4(const Matrix4x4 & mat);

	~Matrix4x4(void){}

public:

	Matrix4x4 operator + (const Matrix4x4 & mat) const;
	Matrix4x4 operator - (const Matrix4x4 & mat) const;
	Matrix4x4 operator * (const Matrix4x4 & mat) const;
	Matrix4x4 operator * (const float value) const;
	Matrix4x4 operator / (const float value) const;
	
	friend Matrix4x4 operator * (float factor, const Matrix4x4 & mat);

	void operator += (const Matrix4x4 & mat);
	void operator -= (const Matrix4x4 & mat);
	void operator *= (const Matrix4x4 & mat);
	void operator *= (const float value);
	void operator /= (const float value);

	bool operator == (const Matrix4x4 & mat) const;
	bool operator != (const Matrix4x4 & mat) const;

	Matrix4x4 operator - (void) const;
	Matrix4x4 operator + (void) const 
	{
		return (*this);
	}

	Vector4 operator * (const Vector4 mat) const;
	inline Vector3 operator * (const Vector3 mat) const
	{
		return Vector3(
		  entries[0]*mat.x
		+ entries[4]*mat.y
		+ entries[8]*mat.z
		+ entries[12],

		  entries[1]*mat.x
		+ entries[5]*mat.y
		+ entries[9]*mat.z
		+ entries[13],

		  entries[2]*mat.x
		+ entries[6]*mat.y
		+ entries[10]*mat.z
		+ entries[14]
		);
	}

	float & operator () (int col, int row){
		return entries[row*4+col];
	} 

	operator float* () const {
		return (float*) this;
	}

	operator const float* () const {
		return (const float*) this;
	}

public:

	void loadIdentity(void);
	void loadZero(void);

	void invert(void);
	void transpose(void);
	void affineInvert(void);
	void invertTranspose(void);
	void affineInvertTranspose(void);

	void scale(const Vector3 & scaleFactor);
	void rotate(const Vector3 & axis, const float angle);
	void translate(const Vector3 translate);

	void setEntry(int position, float value);
	void setScale(const Vector3 & scaleFactor);
	void setUniformScale(const float size);
	void setTranslation(const Vector3 & translation);
	void setTranslationPart(const Vector3 & translation);
	void setRotationAxis(const float angle, const Vector3 & axis);
	void setRotationX(const float angle);
	void setRotationY(const float angle);
	void setRotationZ(const float angle);
	void setRotationEuler(const float angleX, const float angleY, const float angleZ);
	void setRotationPartEuler(const float angleX, const float angleY, const float angleZ);
	void setRotationPartEuler(const Vector3 & rotations)
	{
		setRotationPartEuler(rotations.x, rotations.y, rotations.z);
	}

	float getEntry(int position) const;

	inline Vector3 getRotatedVector3(const Vector3 & vec) const
	{
		return Vector3(
			entries[0]*vec.x + entries[4]*vec.y + entries[8]*vec.z,
			entries[1]*vec.x + entries[5]*vec.y + entries[9]*vec.z,
			entries[2]*vec.x + entries[6]*vec.y + entries[10]*vec.z
		);
	}
	
	Vector3 getInverseRotatedVector3(const Vector3 & vec) const;
	Vector3 getTranslatedVector3(const Vector3 & vec) const;
	Vector3 getInversetranslatedVector3(const Vector3 & vec) const;

	inline Vector3 getTranslationPart(void) const { return Vector3(entries[12], entries[13], entries[14]); }

	Vector3 getEulerAngles(void) const;

	Vector3 getScale(void) const;
	
	Vector4 getRow(int position) const;
	Vector4 getColumn(int position) const;

	Matrix4x4 getInverse(void) const;
	Matrix4x4 getTranspose(void) const;
	Matrix4x4 getInversetranspose(void) const;
	Matrix4x4 getAffineInverse(void) const;
	Matrix4x4 getAffineInverseTranspose(void) const;

	void rotateVector3(Vector3 & vec) const
	{
		vec = getRotatedVector3(vec);
	}

	void inverseRotateVector3(Vector3 & vec) const
	{
		vec = getInverseRotatedVector3(vec);
	}

	void translateVector3(Vector3 & vec) const
	{
		vec = getTranslatedVector3(vec);
	}

	void inversetranslateVector3(Vector3 & vec) const
	{
		vec = getInversetranslatedVector3(vec);
	}
};
}
#endif

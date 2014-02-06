/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MiOSStandardRenderer.h
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


#ifndef _M_STANDARD_RENDERER_H
#define _M_STANDARD_RENDERER_H


#define MAX_LOCAL_LIGHTS 1
#define MAX_SHADOW_LIGHTS 64
#define MAX_DEFAULT_FXS 16


// Shadow Light
class MShadowLight
{
public:
	MShadowLight(void):shadowTexture(0), score(0){}
	int score;
	unsigned int shadowTexture;
	unsigned int shadowQuality;
	float biasUnity;
	MMatrix4x4 shadowMatrix;
};


// Standard Renderer
class MStandardRenderer : public MRenderer
{
public:
	
	MStandardRenderer(void);
	~MStandardRenderer(void);
	
private:
	
	// globals
	bool m_forceNoFX;
	unsigned int m_fboId;
	int m_lightShadow[4];
	int m_lightShadowTexture[4];
	float m_lightShadowBias[4];
	float m_lightShadowBlur[4];
	MMatrix4x4 m_lightShadowMatrix[4];
	MOCamera * m_currentCamera;
	
	// shadow lights
	unsigned int m_randTexture;
	map<unsigned long, MShadowLight> m_shadowLights;
	
	// skin cache
	unsigned int m_verticesNumber;
	unsigned int m_normalsNumber;
	unsigned int m_tangentsNumber;
	MVector3 * m_vertices;
	MVector3 * m_normals;
	MVector3 * m_tangents;
	
	// default FXs
	unsigned int m_FXsNumber;
	unsigned int m_vertShaders[MAX_DEFAULT_FXS];
	unsigned int m_fragShaders[MAX_DEFAULT_FXS];
	unsigned int m_FXs[MAX_DEFAULT_FXS];
	
private:
	
	void addFX(const char * vert, const char * frag);
	void updateSkinning(MMesh * mesh, MArmature * armature);
	void drawDisplay(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices, MVector3 * normals, MVector3 * tangents, MColor * colors);
	//void drawDisplayTriangles(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices);
	void drawOpaques(MSubMesh * subMesh, MArmature * armature);
	void drawTransparents(MSubMesh * subMesh, MArmature * armature);
	void setShadowMatrix(MMatrix4x4 * matrix);
	void updateVisibility(MScene * scene, MOCamera * camera);
	void enableFog(MOCamera * camera);
	
	float getDistanceToCam(MOCamera * camera, const MVector3 & pos);
	
	// shadow lights
	MShadowLight * createShadowLight(MOLight * light);
	void destroyUnusedShadowLights(void);
	void decreaseShadowLights(void);
	
	// skin cache
	MVector3 * getVertices(unsigned int size);
	MVector3 * getNormals(unsigned int size);
	MVector3 * getTangents(unsigned int size);
	
	// text
	void drawText(MOText * textObj);
	
public:
	
	// destroy
	void destroy(void);
	
	// get new
	static MRenderer * getNew(void);

	// name
	static const char * getStaticName(void){ return "StandardRenderer"; }
	const char * getName(void){ return getStaticName(); }
	
	// draw
	void drawScene(MScene * scene, MOCamera * camera);
};

#endif
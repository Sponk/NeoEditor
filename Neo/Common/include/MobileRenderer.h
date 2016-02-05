//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __MOBILE_RENDERER_H
#define __MOBILE_RENDERER_H

#include "config.h"

#define MAX_ENTITY_LIGHTS 256
#define MAX_SHADOW_LIGHTS 64
#define MAX_DEFAULT_FXS 16
#define MAX_OPAQUE 4096
#define MAX_TRANSP 2048

namespace Neo
{

// Entity light
struct EntityLight
{
	Box3d lightBox;
	OLight * light;
};


// SubMesh pass
class SubMeshPass
{
public:
	SubMeshPass(void):occlusionQuery(0){}

	unsigned int occlusionQuery;
	unsigned int subMeshId;
	unsigned int lightsNumber;
	Object3d * object;
	OLight * lights[4];
};


// Shadow Light
class ShadowLight
{
public:
	ShadowLight(void):shadowTexture(0), score(0){}

	int score;
	unsigned int shadowTexture;
	unsigned int shadowQuality;
	float biasUnity;
	Matrix4x4 shadowMatrix;
};


// Standard Renderer
class NEO_COMMON_EXPORT MobileRenderer : public Renderer
{
public:

	MobileRenderer(void);
	~MobileRenderer(void);

private:

	// globals
	bool m_forceNoFX;
	unsigned int m_fboId;
	int m_lightShadow[4];
	int m_lightShadowTexture[4];
	float m_lightShadowBias[4];
	float m_lightShadowBlur[4];

	Matrix4x4 m_currModelViewMatrix;
	Matrix4x4 m_lightShadowMatrix[4];
	OCamera * m_currentCamera;

	// shadow lights
	unsigned int m_randTexture;
	map<unsigned long, ShadowLight> m_shadowLights;

	// skin cache
	unsigned int m_verticesNumber;
	unsigned int m_normalsNumber;
	unsigned int m_tangentsNumber;
	Vector3 * m_vertices;
	Vector3 * m_normals;
	Vector3 * m_tangents;

	// default FXs
	unsigned int m_FXsNumber;
	unsigned int m_vertShaders[MAX_DEFAULT_FXS];
	unsigned int m_fragShaders[MAX_DEFAULT_FXS];
	unsigned int m_FXs[MAX_DEFAULT_FXS];

	// lists
	int m_transpSortList[MAX_TRANSP];
	int m_opaqueSortList[MAX_OPAQUE];
	float m_transpSortZList[MAX_TRANSP];
	float m_opaqueSortZList[MAX_OPAQUE];
	SubMeshPass m_transpList[MAX_TRANSP];
	SubMeshPass m_opaqueList[MAX_OPAQUE];

	// lights list
	int m_entityLightsList[MAX_ENTITY_LIGHTS];
	float m_entityLightsZList[MAX_ENTITY_LIGHTS];
	EntityLight m_entityLights[MAX_ENTITY_LIGHTS];

private:

	void addFX(const char * vert, const char * frag);
	void updateSkinning(Mesh * mesh, Armature * armature);
	void drawDisplay(SubMesh * subMesh, MaterialDisplay * display, Vector3 * vertices, Vector3 * normals, Vector3 * tangents, Color * colors);
	//void drawDisplayTriangles(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices);
	void drawOpaques(SubMesh * subMesh, Armature * armature);
	void drawTransparents(SubMesh * subMesh, Armature * armature);
	void setShadowMatrix(Matrix4x4 * matrix, OCamera * camera);
	void updateVisibility(Scene * scene, OCamera * camera);
	void enableFog(OCamera * camera);

	void initVBO(SubMesh * subMesh);

	float getDistanceToCam(OCamera * camera, const Vector3 & pos);

	// shadow lights
	ShadowLight * createShadowLight(OLight * light);
	void destroyUnusedShadowLights(void);
	void decreaseShadowLights(void);

	// skin cache
	Vector3 * getVertices(unsigned int size);
	Vector3 * getNormals(unsigned int size);
	Vector3 * getTangents(unsigned int size);

	// subMesh
	void prepareSubMesh(Scene * scene, OCamera * camera, OEntity * entity, SubMesh * subMesh);

public:

	// destroy
	void destroy(void);

	// get new
	static Renderer * getNew(void);

	// name
	static const char * getStaticName(void){ return "MobileRenderer"; }
	const char * getName(void){ return getStaticName(); }

	// draw
	void drawScene(Scene * scene, OCamera * camera);

	// extra
	void drawText(OText * textObj);
};
}
#endif

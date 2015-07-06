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

#ifndef __OCAMERA_H
#define __OCAMERA_H

namespace Neo
{
class NEO_ENGINE_EXPORT OCamera : public Object3d
{
private:

	// properties
	bool m_fog;
    Vector3 m_fogColor;
	bool m_ortho;
	float m_fov;
	float m_fogDistance;
	float m_clippingNear;
	float m_clippingFar;
	Vector3 m_clearColor;

	// current matrices
	Matrix4x4 m_currentViewMatrix; // current model view matrix
	Matrix4x4 m_currentProjMatrix; // current projection matrix

	// current viewport
	int m_currentViewport[4]; // 0=x 1=y 2=width 3=height

	// frustum (camera volume for elimination)
	Frustum m_frustum;

	// scene layer (used for GUI etc)
	unsigned int m_sceneLayer;

	// render to texture
	TextureRef * m_renderColorTexture;
	TextureRef * m_renderDepthTexture;

    Skybox m_skybox;

public:

	// constructor
	OCamera(void);

	// destructor
	~OCamera(void);

	// copy constructor
	OCamera(const OCamera & camera);

public:

	// type
	int getType(void){ return M_OBJECT3D_CAMERA; }

	// properties
	inline void enableFog(const bool fog){ m_fog = fog; }
    inline Vector3 getFogColor() { return m_fogColor; }
    inline void setFogColor(Vector3 color) { m_fogColor = color; }
	inline void enableOrtho(const bool ortho){ m_ortho = ortho; }
	inline void setFov(const float fov){ m_fov = fov; }
	inline void setFogDistance(const float fogDistance){ m_fogDistance = fogDistance; }
	inline void setClippingNear(const float clippingNear){ m_clippingNear = clippingNear; }
	inline void setClippingFar(const float clippingFar){ m_clippingFar = clippingFar; }
	inline bool isOrtho(void){ return m_ortho; }
	inline bool hasFog(void){ return m_fog; }
	inline float getClippingNear(void){ return m_clippingNear; }
	inline float getClippingFar(void){ return m_clippingFar; }
	inline float getFov(void){ return m_fov; }
	inline float getFogDistance(void){ return m_fogDistance; }
	inline void setClearColor(Vector3 clearColor) { m_clearColor = clearColor; }
	inline Vector3 getClearColor(void) const { return m_clearColor; }

	// projection
	Vector3 getProjectedPoint(const Vector3 & point) const;
	Vector3 getUnProjectedPoint(const Vector3 & point) const;

	// matrices
	inline Matrix4x4 * getCurrentViewMatrix(void){ return &m_currentViewMatrix; }
	inline Matrix4x4 * getCurrentProjMatrix(void){ return &m_currentProjMatrix; }

	// viewport
	int * getCurrentViewport(void){ return m_currentViewport; }

	// frustum
	inline Frustum * getFrustum(void){ return &m_frustum; }

	// scene layer
	inline void setSceneLayer(unsigned int sceneLayer){ m_sceneLayer = sceneLayer; }
	inline unsigned int getSceneLayer(void){ return m_sceneLayer; }

	// render to texture
	inline void setRenderColorTexture(TextureRef * renderColorTexture){ m_renderColorTexture = renderColorTexture; }
	inline void setRenderDepthTexture(TextureRef * renderDepthTexture){ m_renderDepthTexture = renderDepthTexture; }
	inline TextureRef * getRenderColorTexture(void){ return m_renderColorTexture; }
	inline TextureRef * getRenderDepthTexture(void){ return m_renderDepthTexture; }

    inline void drawSkybox() { enable(); m_skybox.drawSkybox(getTransformedPosition(), getTransformedRotation()); }
    inline void loadSkybox(const char* path) { m_skybox.loadSkyboxTextures(path); }
    inline Skybox * getSkybox() { return &m_skybox; }

	// listener
	void updateListener(void);

	// enable
	void enable(void);
	Matrix4x4 setPerspectiveView(float fov, float ratio, float zNear, float zFar);
	Matrix4x4 setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar);
};
}
#endif

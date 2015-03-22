/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MES1Context.h
//
// OpenGL-ES1 Rendering Context
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2011 Anael Seghezzi <www.maratis3d.com>
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


#ifndef _M_ES1_CONTEXT_H
#define _M_ES1_CONTEXT_H

#include <NeoCore.h>
#define MAX_MRCLIGHTS 8


class MES1Context : public RenderingContext
{
private:

	// version
    const char * m_gl_version;
	
	// frame buffer
	unsigned int m_currentFrameBuffer;
	
	// fog
	float m_fogMin, m_fogMax;
	Vector3 m_fogColor;
	
	// lights
	struct
	{
		Vector4 position;
		Vector4 diffuse;
		Vector4 specular;
		Vector4 ambient;
		Vector3 direction;
		float angle, exponent;
		float constant, linear, quadratic;
	}
	m_lights[MAX_MRCLIGHTS];

public:

	MES1Context(void);
	~MES1Context(void);

	// version
	const char * getRendererVersion(void) { return m_gl_version; }
	
	// view
	void setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar);
	void setPerspectiveView(float fov, float ratio, float zNear, float zFar);

	// viewport
	void setViewport(int x, int y, unsigned int width, unsigned int height);

	// clear
	void clear(int buffer);
	void setClearColor(const Vector4 & color);

	// texture
	void enableTexture(void);
	void disableTexture(void);
	void setTextureGenMode(TEX_GEN_MODES mode);
	void setTextureFilterMode(TEX_FILTER_MODES min, TEX_FILTER_MODES mag);
	void setTextureUWrapMode(WRAP_MODES wrap);
	void setTextureVWrapMode(WRAP_MODES wrap);
	void setTextureCombineMode(TEX_COMBINE_MODES combine);
	void bindTexture(unsigned int textureId, const unsigned int multitextureId = 0);
	void createTexture(unsigned int * textureId);
	void deleteTexture(unsigned int * textureId);
	void sendTextureImage(Image * image, bool mipMap, bool filter, bool compress);
	void texImage(unsigned int level, unsigned int width, unsigned int height, VAR_TYPES type, TEX_MODES mode, const void * pixels);
	void texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, VAR_TYPES type, TEX_MODES mode, const void * pixels);
	void generateMipMap(void);

	// frame buffer
	void createFrameBuffer(unsigned int * frameBufferId);
	void deleteFrameBuffer(unsigned int * frameBufferId);
	void bindFrameBuffer(unsigned int frameBufferId);
	void getCurrentFrameBuffer(unsigned int * frameBufferId);
	void attachFrameBufferTexture(FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId);
	void setDrawingBuffers(FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size);
	
	// shaders
	void createVertexShader(unsigned int * shaderId);
	void createPixelShader(unsigned int * shaderId);
	void deleteShader(unsigned int * shaderId);
	void sendShaderSource(unsigned int shaderId, const char * source);

	// FX
	void bindFX(unsigned int fxId);
	void createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId);
	void updateFX(unsigned int fxId);
	void deleteFX(unsigned int * fxId);
	void sendUniformInt(unsigned int fxId, const char * name, int * values, const int count = 1);
	void sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count = 1);
    void sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count = 1);
    void sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count = 1);
	void sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count = 1);
	void sendUniformMatrix(unsigned int fxId, const char * name, Matrix4x4 * matrix, const int count = 1, const bool transpose = false);
	void getAttribLocation(unsigned int fxId, const char * name, int * location);
	
	// arrays
	void enableVertexArray(void);
	void enableColorArray(void);
	void enableNormalArray(void);
	void enableTexCoordArray(void);
	void enableAttribArray(unsigned int location);
	void disableVertexArray(void);
	void disableColorArray(void);
	void disableNormalArray(void);
	void disableTexCoordArray(void);
	void disableAttribArray(unsigned int location);
	void setVertexPointer(VAR_TYPES type, unsigned int components, const void * pointer);
	void setColorPointer(VAR_TYPES type, unsigned int components, const void * pointer);
	void setNormalPointer(VAR_TYPES type, const void * pointer);
	void setTexCoordPointer(VAR_TYPES type, unsigned int components, const void * pointer);
	void setAttribPointer(unsigned int location, VAR_TYPES type, unsigned int components, const void * pointer, const bool normalized = false);

	// draw
	void drawArray(PRIMITIVE_TYPES type, unsigned int begin, unsigned int size);
	void drawElement(PRIMITIVE_TYPES type, unsigned int size, VAR_TYPES indicesType, const void * indices);

	// lines
	void enableLineAntialiasing(void);
	void disableLineAntialiasing(void);

	// material
	void setMaterialDiffuse(const Vector4 & diffuse);
	void setMaterialSpecular(const Vector4 & specular);
	void setMaterialAmbient(const Vector4 & ambient);
	void setMaterialEmit(const Vector4 & emit);
	void setMaterialShininess(float shininess);

	// scissor
	void enableScissorTest(void);
	void disableScissorTest(void);
	void setScissor(int x, int y, unsigned int width, unsigned int height);

	// color
	void setColor(const Color & color);
	void setColor3(const Vector3 & color);
	void setColor4(const Vector4 & color);

	// masks
	void setColorMask(bool r, bool g, bool b, bool a);
	void setDepthMask(bool depth);

	// alpha
	void setAlphaTest(float value);
	
	// depth
	void enableDepthTest(void);
	void disableDepthTest(void);
	void setDepthMode(DEPTH_MODES mode);

	// stencil
	void enableStencilTest(void);
	void disableStencilTest(void);

	// cull face
	void enableCullFace(void);
	void disableCullFace(void);
	void setCullMode(CULL_MODES mode);

	// matrix
	void loadIdentity(void);
	void setMatrixMode(MATRIX_MODES mode);
	void pushMatrix(void);
	void popMatrix(void);
	void multMatrix(const Matrix4x4 * matrix);
	void translate(const Vector3 & position);
	void rotate(const Vector3 & axis, float angle);
	void scale(const Vector3 & scale);
	void getViewport(int * viewport);
	void getModelViewMatrix(Matrix4x4 * matrix);
	void getProjectionMatrix(Matrix4x4 * matrix);
	void getTextureMatrix(Matrix4x4 * matrix);

	// fog
	void enableFog(void);
	void disableFog(void);
	void setFogColor(const Vector3 & color);
	void setFogDistance(float min, float max);
	void getFogColor(Vector3 * color);
	void getFogDistance(float * min, float * max);

	// lighting
	void enableLighting(void);
	void disableLighting(void);
	void enableLight(unsigned int id);
	void disableLight(unsigned int id);
	void setLightPosition(unsigned int id, const Vector4 & position);
	void setLightDiffuse(unsigned int id, const Vector4 & diffuse);
	void setLightSpecular(unsigned int id, const Vector4 & specular);
	void setLightAmbient(unsigned int id, const Vector4 & ambient);
	void setLightAttenuation(unsigned int id, float constant, float linear, float quadratic);
	void setLightSpotDirection(unsigned int id, const Vector3 & direction);
	void setLightSpotAngle(unsigned int id, float angle);
	void setLightSpotExponent(unsigned int id, float exponent);
	void getLightPosition(unsigned int id, Vector4 * position);
	void getLightDiffuse(unsigned int id, Vector4 * diffuse);
	void getLightSpecular(unsigned int id, Vector4 * specular);
	void getLightAmbient(unsigned int id, Vector4 * ambient);
	void getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic);
	void getLightSpotDirection(unsigned int id, Vector3 * direction);
	void getLightSpotAngle(unsigned int id, float * angle);
	void getLightSpotExponent(unsigned int id, float * exponent);
	
	// blending
	void enableBlending(void);
	void disableBlending(void);
	void setBlendingMode(BLENDING_MODES mode);
};

#endif

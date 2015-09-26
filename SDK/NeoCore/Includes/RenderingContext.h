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


#ifndef __RENDERING_CONTEXT_H
#define __RENDERING_CONTEXT_H

namespace Neo
{
// blending modes
enum BLENDING_MODES
{
	BLENDING_NONE = 0,
	BLENDING_ALPHA,
	BLENDING_ADD,
	BLENDING_SUB,
	BLENDING_LIGHT,
	BLENDING_PRODUCT
};

// depth modes
enum DEPTH_MODES
{
	DEPTH_NONE = 0,
	DEPTH_ALWAYS,
	DEPTH_LESS,
	DEPTH_GREATER,
	DEPTH_EQUAL,
	DEPTH_LEQUAL,
	DEPTH_GEQUAL,
	DEPTH_NOTEQUAL
};

// matrix modes
enum MATRIX_MODES
{
	MATRIX_MODELVIEW = 0,
	MATRIX_PROJECTION,
	MATRIX_TEXTURE,
};

// primitives types
enum PRIMITIVE_TYPES
{
	PRIMITIVE_POINTS = 0,
	PRIMITIVE_LINES,
	PRIMITIVE_LINE_LOOP,
	PRIMITIVE_LINE_STRIP,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_TRIANGLE_STRIP,
	PRIMITIVE_TRIANGLE_FAN
};

// buffer types
enum BUFFER_TYPES
{
	BUFFER_COLOR = 2,
	BUFFER_DEPTH = 4,
	BUFFER_STENCIL = 8
};

// texture image mode
enum TEX_MODES
{
	TEX_DEPTH = 0,
	TEX_R = 1,
	TEX_RG = 2,
	TEX_RGB = 3,
	TEX_RGBA = 4
};

// texture gen modes
enum TEX_GEN_MODES
{
	TEX_GEN_NONE = 0,
	TEX_GEN_SPHERE_MAP,
	TEX_GEN_CUBE_MAP
};

// texture combine modes
enum TEX_COMBINE_MODES
{
	TEX_COMBINE_REPLACE = 0,
	TEX_COMBINE_MODULATE,
	TEX_COMBINE_ALPHA,
	TEX_COMBINE_ADD,
	TEX_COMBINE_SUB,
	TEX_COMBINE_DOT
};

// texture filtering modes
enum TEX_FILTER_MODES
{
	TEX_FILTER_NEAREST = 0,
	TEX_FILTER_NEAREST_MIPMAP_NEAREST,
	TEX_FILTER_NEAREST_MIPMAP_LINEAR,
	TEX_FILTER_LINEAR,
	TEX_FILTER_LINEAR_MIPMAP_NEAREST,
	TEX_FILTER_LINEAR_MIPMAP_LINEAR,
};

// wrap modes
enum WRAP_MODES
{
	WRAP_REPEAT = 0,
	WRAP_CLAMP
};

// cull modes
enum CULL_MODES
{
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
	CULL_FRONT_BACK
};

// types
enum VAR_TYPES
{
	VAR_BOOL = 0,
	VAR_BYTE,
	VAR_UBYTE,
	VAR_SHORT,
	VAR_USHORT,
	VAR_INT,
	VAR_UINT,
	VAR_FLOAT,
	VAR_DOUBLE
};

enum FRAME_BUFFER_ATTACHMENT
{
	ATTACH_DEPTH = 0,
	ATTACH_STENCIL,
	ATTACH_COLOR0,
	ATTACH_COLOR1,
	ATTACH_COLOR2,
	ATTACH_COLOR3,
	ATTACH_COLOR4,
	ATTACH_COLOR5,
	ATTACH_COLOR6,
	ATTACH_COLOR7
};

enum RENDER_BUFFER_MODES
{
	RENDER_DEPTH = 0,
	RENDER_STENCIL,
	RENDER_DEPTH_STENCIL
};

// stencil funcs
enum STENCIL_FUNCS
{
	STENCIL_ALWAYS = 0,
	STENCIL_NEVER,
	STENCIL_EQUAL,
	STENCIL_NOTEQUAL,
	STENCIL_LESS,
	STENCIL_LEQUAL,
	STENCIL_GREATER,
	STENCIL_GEQUAL
};

// stencil ops
enum STENCIL_OPS
{
	STENCIL_KEEP = 0,
	STENCIL_INVERT,
	STENCIL_DECR,
	STENCIL_INCR,
	STENCIL_REPLACE
};

// VBO types
enum VBO_TYPES
{
	VBO_ARRAY = 0,
	VBO_ELEMENT_ARRAY
};

// VBO modes
enum VBO_MODES
{
	VBO_STATIC = 0,
	VBO_DYNAMIC,
	VBO_STREAM
};


// rendering context
/**
 * @brief The RenderingContext class defines an abstraction for the 3D rendering API like OpenGL
 */
class NEO_CORE_EXPORT RenderingContext
{
public:

	// destructor
	virtual ~RenderingContext(void){}

	//  version
    /**
     * @brief getRendererVersion returns the renderer.
     * @return
     */
    virtual const char* getRendererVersion() = 0;

	// view
    /**
     * @brief setOrthoView Multiplies the current view matrix with the ortho matrix defined by the arguments.
     *
     * This method will produce a box like view frustum.
     * In this process a perspective view turns into an ortho view which can be used for drawing 2D graphics
     * or isometric graphics.
     *
     * @param left Distance to the left plane
     * @param right Distance to the right plane
     * @param bottom Distance to the bottom plane
     * @param top Distance to the top plane
     * @param zNear Distance to the near plane. Defines how near objects will still be rendered.
     * @param zFar Distance to the far plane. Defines how far objects will still be rendered
     */
	virtual void setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar) = 0;

    /**
     * @brief setPerspectiveView Multiplies the current view matrix with the matrix defined by the arguments
     *
     * This method will produce a pyramid like view frustum.
     * In this process the the view turns in a perspective view which can be used for all sorts of 3D graphics.
     *
     * @param fov The field of view of the new frustum in degrees. On consoles this value often is 60°, on PC it often defaults to 90°
     * @param ratio The radio between the width and the height of the screen (ratio = width/height). This can be used to adjust the fov in x direction.
     * @param zNear Distance to the near plane. Defines how near objects will still be rendered.
     * @param zFar Distance to the far plane. Defines how far objects will still be rendered
     */
	virtual void setPerspectiveView(float fov, float ratio, float zNear, float zFar) = 0;

	// viewport
    /**
     * @brief setViewport Sets the viewport that is used to render to.
     *
     * Sets the current viewport to the area defined by the arguments.
     * This can be used to display split screen content.
     * Default is x = 0, y = 0, width = screenWidth, height = screenHeight
     *
     * @param x X position on the screen.
     * @param y Y position on the screen.
     * @param width Width of the area.
     * @param height Height of the area.
     */
	virtual void setViewport(int x, int y, unsigned int width, unsigned int height) = 0;

	// clear
    /**
     * @brief clear Clear the buffer defined by the argument.
     *
     * Argument has to be in the enumeration M_BUFFER_TYPES.
     * They can be combined by using bit wise or: clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);
     * The color to clear the buffer is set by MRenderingContext::setClearColor.
     *
     * @param buffer The buffer(s) to clear.
     */
	virtual void clear(int buffer) = 0;

    /**
     * @brief setClearColor Sets the clear color used by MRenderingContext::clear
     * @param color The rgba color.
     */
	virtual void setClearColor(const Vector4 & color) = 0;

	virtual void init() = 0;

	// texture
	virtual void enableTexture(void) = 0;
	virtual void disableTexture(void) = 0;
	virtual void setTextureGenMode(TEX_GEN_MODES mode) = 0;
	virtual void setTextureFilterMode(TEX_FILTER_MODES min, TEX_FILTER_MODES mag) = 0;
	virtual void setTextureUWrapMode(WRAP_MODES wrap) = 0;
	virtual void setTextureVWrapMode(WRAP_MODES wrap) = 0;
	virtual void setTextureCombineMode(TEX_COMBINE_MODES combine) = 0;
	virtual void bindTexture(unsigned int textureId, const unsigned int multitextureId = 0) = 0;
	virtual void createTexture(unsigned int * textureId) = 0;
	virtual void deleteTexture(unsigned int * textureId) = 0;
	virtual void sendTextureImage(Image * image, bool mipMap, bool filter, bool compress) = 0;
	virtual void texImage(unsigned int level, unsigned int width, unsigned int height, VAR_TYPES type, TEX_MODES mode, const void * pixels) = 0;
	virtual void texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, VAR_TYPES type, TEX_MODES mode, const void * pixels) = 0;
	virtual void generateMipMap(void) = 0;
	virtual void getTexImage(unsigned int level, Image * image){}

	// frame buffer
	virtual void createFrameBuffer(unsigned int * frameBufferId) = 0;
	virtual void deleteFrameBuffer(unsigned int * frameBufferId) = 0;
	virtual void bindFrameBuffer(unsigned int frameBufferId) = 0;
	virtual void getCurrentFrameBuffer(unsigned int * frameBufferId) = 0;
	virtual void attachFrameBufferTexture(FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId) = 0;
	virtual void attachFrameBufferRB(FRAME_BUFFER_ATTACHMENT attachment, unsigned int renderBufferId){}
	virtual void setDrawingBuffers(FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size) = 0;

	// render buffer
	virtual void createRenderBuffer(unsigned int * renderBufferId){}
	virtual void deleteRenderBuffer(unsigned int * renderBufferId){}
	virtual void bindRenderBuffer(unsigned int renderBufferId){}
	virtual void setRenderBuffer(RENDER_BUFFER_MODES mode, unsigned int width, unsigned int height){}

	// shaders
	virtual void createVertexShader(unsigned int * shaderId) = 0;
	virtual void createPixelShader(unsigned int * shaderId) = 0;
	virtual void deleteShader(unsigned int * shaderId) = 0;
    virtual bool sendShaderSource(unsigned int shaderId, const char * source) = 0;

	// FX
	virtual void bindFX(unsigned int fxId) = 0;
	virtual void createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId) = 0;
	virtual void updateFX(unsigned int fxId) = 0;
	virtual void deleteFX(unsigned int * fxId) = 0;
	virtual void sendUniformInt(unsigned int fxId, const char * name, int * values, const int count = 1) = 0;
	virtual void sendUniformFloat(unsigned int fxId, const char * name, float * value, const int count = 1) = 0;
    virtual void sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count = 1) = 0;
	virtual void sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count = 1) = 0;
	virtual void sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count = 1) = 0;
	virtual void sendUniformMatrix(unsigned int fxId, const char * name, Matrix4x4 * matrix, const int count = 1, const bool transpose = false) = 0;
	virtual void getAttribLocation(unsigned int fxId, const char * name, int * location) = 0;

	// VBO
	virtual void createVBO(unsigned int * vboId){}
	virtual void deleteVBO(unsigned int * vboId){}
	virtual void bindVBO(VBO_TYPES type, unsigned int vboId){}
	virtual void setVBO(VBO_TYPES type, const void * data, unsigned int size, VBO_MODES mode){}
	virtual void setVBOSubData(VBO_TYPES type, unsigned int offset, const void * data, unsigned int size){}
	
	// arrays
	virtual void enableVertexArray(void) = 0;
	virtual void enableColorArray(void) = 0;
	virtual void enableNormalArray(void) = 0;
	virtual void enableTexCoordArray(void) = 0;
	virtual void enableAttribArray(unsigned int location) = 0;
	virtual void disableVertexArray(void) = 0;
	virtual void disableColorArray(void) = 0;
	virtual void disableNormalArray(void) = 0;
	virtual void disableTexCoordArray(void) = 0;
	virtual void disableAttribArray(unsigned int location) = 0;
	virtual void setVertexPointer(VAR_TYPES type, unsigned int components, const void * pointer) = 0;
	virtual void setColorPointer(VAR_TYPES type, unsigned int components, const void * pointer) = 0;
	virtual void setNormalPointer(VAR_TYPES type, const void * pointer) = 0;
	virtual void setTexCoordPointer(VAR_TYPES type, unsigned int components, const void * pointer) = 0;
	virtual void setAttribPointer(unsigned int location, VAR_TYPES type, unsigned int components, const void * pointer, const bool normalized = false) = 0;

	// draw
	virtual void drawArray(PRIMITIVE_TYPES type, unsigned int begin, unsigned int size) = 0;
	virtual void drawElement(PRIMITIVE_TYPES type, unsigned int size, VAR_TYPES indicesType, const void * indices) = 0;

	// lines
	virtual void enableLineAntialiasing(void) = 0;
	virtual void disableLineAntialiasing(void) = 0;

	// material
	virtual void setMaterialDiffuse(const Vector4 & diffuse) = 0;
	virtual void setMaterialSpecular(const Vector4 & specular) = 0;
	virtual void setMaterialAmbient(const Vector4 & ambient) = 0;
	virtual void setMaterialEmit(const Vector4 & emit) = 0;
	virtual void setMaterialShininess(float shininess) = 0;

	// scissor
	virtual void enableScissorTest(void) = 0;
	virtual void disableScissorTest(void) = 0;
	virtual void setScissor(int x, int y, unsigned int width, unsigned int height) = 0;

	// color
	virtual void setColor(const Color & color) = 0;
	virtual void setColor3(const Vector3 & color) = 0;
	virtual void setColor4(const Vector4 & color) = 0;

	// masks
	virtual void setColorMask(bool r, bool g, bool b, bool a) = 0;
	virtual void setDepthMask(bool depth) = 0;

	// alpha
	virtual void setAlphaTest(float value) = 0;

	// depth
	virtual void enableDepthTest(void) = 0;
	virtual void disableDepthTest(void) = 0;
	virtual void setDepthMode(DEPTH_MODES mode) = 0;
	virtual void enablePolygonOffset(float x, float y) = 0;
	virtual void disablePolygonOffset() = 0;

	// stencil
	virtual void enableStencilTest(void) = 0;
	virtual void disableStencilTest(void) = 0;
	virtual void setStencilFunc(STENCIL_FUNCS func, int ref=0){};
	virtual void setStencilOp(STENCIL_OPS op){};

	// cull face
	virtual void enableCullFace(void) = 0;
	virtual void disableCullFace(void) = 0;
	virtual void setCullMode(CULL_MODES mode) = 0;

	// occlusion
	virtual void createQuery(unsigned int * queryId){}
	virtual void deleteQuery(unsigned int * queryId){}
	virtual void beginQuery(unsigned int queryId){}
	virtual void endQuery(void){}
	virtual void getQueryResult(unsigned int queryId, unsigned int * result){}

	// matrix
	virtual void loadIdentity(void) = 0;
	virtual void setMatrixMode(MATRIX_MODES mode) = 0;
	virtual void pushMatrix(void) = 0;
	virtual void popMatrix(void) = 0;
	virtual void multMatrix(const Matrix4x4 * matrix) = 0;
	virtual void translate(const Vector3 & position) = 0;
	virtual void rotate(const Vector3 & axis, float angle) = 0;
	virtual void scale(const Vector3 & scale) = 0;
	virtual void getViewport(int * viewport) = 0;
	virtual void getModelViewMatrix(Matrix4x4 * matrix) = 0;
	virtual void getProjectionMatrix(Matrix4x4 * matrix) = 0;
	virtual void getTextureMatrix(Matrix4x4 * matrix) = 0;

	// fog
	virtual void enableFog(void) = 0;
	virtual void disableFog(void) = 0;
	virtual void setFogColor(const Vector3 & color) = 0;
	virtual void setFogDistance(float min, float max) = 0;
	virtual void getFogColor(Vector3 * color) = 0;
	virtual void getFogDistance(float * min, float * max) = 0;

	// lighting
	virtual void enableLighting(void) = 0;
	virtual void disableLighting(void) = 0;
	virtual void enableLight(unsigned int id) = 0;
	virtual void disableLight(unsigned int id) = 0;
	virtual void setLightPosition(unsigned int id, const Vector4 & position) = 0;
	virtual void setLightDiffuse(unsigned int id, const Vector4 & diffuse) = 0;
	virtual void setLightSpecular(unsigned int id, const Vector4 & specular) = 0;
	virtual void setLightAmbient(unsigned int id, const Vector4 & ambient) = 0;
	virtual void setLightAttenuation(unsigned int id, float constant, float linear, float quadratic) = 0;
	virtual void setLightSpotDirection(unsigned int id, const Vector3 & direction) = 0;
	virtual void setLightSpotAngle(unsigned int id, float angle) = 0;
	virtual void setLightSpotExponent(unsigned int id, float exponent) = 0;
	virtual void getLightPosition(unsigned int id, Vector4 * position) = 0;
	virtual void getLightDiffuse(unsigned int id, Vector4 * diffuse) = 0;
	virtual void getLightSpecular(unsigned int id, Vector4 * specular) = 0;
	virtual void getLightAmbient(unsigned int id, Vector4 * ambient) = 0;
	virtual void getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic) = 0;
	virtual void getLightSpotDirection(unsigned int id, Vector3 * direction) = 0;
	virtual void getLightSpotAngle(unsigned int id, float * angle) = 0;
	virtual void getLightSpotExponent(unsigned int id, float * exponent) = 0;

	// blending
	virtual void enableBlending(void) = 0;
	virtual void disableBlending(void) = 0;
	virtual void setBlendingMode(BLENDING_MODES mode) = 0;

	virtual void createVAO(unsigned int* vaoId) {}
	virtual void deleteVAO(unsigned int* vaoId) {}
	virtual void bindVAO(unsigned int vaoId) {}

	virtual void selectSubroutine(unsigned int fx, unsigned int type, const char* routine) {}
	virtual void setPolygonMode(PRIMITIVE_TYPES t) {};
	
    // point size
    virtual void setPointSize(float size) = 0;
};
}
#endif

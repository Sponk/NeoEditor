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

#ifdef USE_GLES
#ifdef __APPLE__
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// Really?
#include <GLES/gl.h>
#endif

#ifdef EMSCRIPTEN
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
// Really?
#include <GLES/gl.h>
#endif

#ifdef LINUX
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include <NeoEngine.h>
#include "ES2Context.h"

using namespace Neo;

static int g_GLversion = 0;
static float maxAnisotropy = 0.0f;

static GLenum returnGLType(M_TYPES type)
{
    switch(type)
    {
        default:

            //case M_BOOL:
            //return GL_BOOL;

        case M_BYTE:
            return GL_BYTE;

        case M_UBYTE:
            return GL_UNSIGNED_BYTE;

        case M_SHORT:
            return GL_SHORT;

        case M_USHORT:
            return GL_UNSIGNED_SHORT;

        case M_INT:
            return GL_INT;

        case M_UINT:
            return GL_UNSIGNED_INT;

        case M_FLOAT:
            return GL_FLOAT;

            //case M_DOUBLE:
            //return GL_DOUBLE;
    }
}

static GLenum returnPrimitiveType(M_PRIMITIVE_TYPES type)
{
    switch(type)
    {
        default:
        case M_PRIMITIVE_LINES:
            return GL_LINES;

        case M_PRIMITIVE_LINE_LOOP:
            return GL_LINE_LOOP;

        case M_PRIMITIVE_LINE_STRIP:
            return GL_LINE_STRIP;

        case M_PRIMITIVE_TRIANGLES:
            return GL_TRIANGLES;

        case M_PRIMITIVE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;

        case M_PRIMITIVE_POINTS:
            return GL_POINTS;
    }
}

static GLenum returnTexFilterMode(M_TEX_FILTER_MODES mode)
{
    switch(mode)
    {
        default:
        case M_TEX_FILTER_NEAREST:
            return GL_NEAREST;

        case M_TEX_FILTER_NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;

        case M_TEX_FILTER_NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;

        case M_TEX_FILTER_LINEAR:
            return GL_LINEAR;

        case M_TEX_FILTER_LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;

        case M_TEX_FILTER_LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
}

static GLenum returnTexMode(M_TEX_MODES mode)
{
    switch(mode)
    {
        //case M_R:
        //	return GL_R;

        //case M_RG:
        //	return GL_RG;

        default:
        case M_RGB:
            return GL_RGB;

        case M_RGBA:
            return GL_RGBA;

        case M_DEPTH:
            return GL_DEPTH_COMPONENT;
    }
}

static GLenum returnAttachType(M_FRAME_BUFFER_ATTACHMENT type)
{
    if(type == M_ATTACH_DEPTH)
        return GL_DEPTH_ATTACHMENT;
    else if(type == M_ATTACH_STENCIL)
        return GL_STENCIL_ATTACHMENT;
    else
        return GL_COLOR_ATTACHMENT0 + ((int)type - 2);
}

ES2Context::ES2Context(void):
m_currentFrameBuffer(0)
{

}

void ES2Context::init()
{
    m_matrixStep = 0;
    m_matrixMode = M_MATRIX_MODELVIEW;

    // version
    const char * version = (const char *)glGetString(GL_VERSION);
    if(version)
    {
        m_gl_version=version;
        sscanf(version, "%d", &g_GLversion);
    }

    // init cull face (back)
    enableCullFace();
    setCullMode(M_CULL_BACK);

    // depth
    enableDepthTest();
    setDepthMode(M_DEPTH_LEQUAL);
    glClearDepthf(1.0f);

	// line
    glLineWidth(1);

    // stencil
    glClearStencil(0);

    // pixel pack/unpack
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // anisotropic filtering
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
}

ES2Context::~ES2Context(void)
{}

// view
void ES2Context::setPerspectiveView(float fov, float ratio, float zNear, float zFar)
{
    Matrix4x4 matrix;

    float ymax, xmax;
    ymax = zNear * tanf((float)(fov * M_PI / 360.0f));
    xmax = ymax * ratio;

    float left = -xmax;
    float right = xmax;
    float bottom = -ymax;
    float top = ymax;

    float temp, temp2, temp3, temp4;
    temp = 2.0f * zNear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zFar - zNear;
    matrix.entries[0] = temp / temp2;
    matrix.entries[1] = 0.0;
    matrix.entries[2] = 0.0;
    matrix.entries[3] = 0.0;
    matrix.entries[4] = 0.0;
    matrix.entries[5] = temp / temp3;
    matrix.entries[6] = 0.0;
    matrix.entries[7] = 0.0;
    matrix.entries[8] = (right + left) / temp2;
    matrix.entries[9] = (top + bottom) / temp3;
    matrix.entries[10] = (-zFar - zNear) / temp4;
    matrix.entries[11] = -1.0;
    matrix.entries[12] = 0.0;
    matrix.entries[13] = 0.0;
    matrix.entries[14] = (-temp * zFar) / temp4;
    matrix.entries[15] = 0.0;

    multMatrix(&matrix);
}

void ES2Context::setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar)
{
    if(right == left || top == bottom || zFar == zNear)
    {
        // GL_INVALID_VALUE;
        return;
    }

    float tx = - (right + left)/(right - left);
    float ty = - (top + bottom)/(top - bottom);
    float tz = - (zFar + zNear)/(zFar - zNear);

    Matrix4x4 matrix(
        2.0f/(right-left), 0.0f, 0.0f, 0.0f,
              0.0f, 2.0f/(top-bottom), 0.0f, 0.0f,
              0.0f, 0.0f, -2.0f/(zFar-zNear), 0.0f,
              tx, ty, tz, 1.0f
    );

    multMatrix(&matrix);
}

// viewport
void ES2Context::setViewport(int x, int y, unsigned int width, unsigned int height){
    glViewport(x, y, width, height);
}

// clear
void ES2Context::clear(int buffer)
{
    switch(buffer)
    {
        case BUFFER_COLOR:
            glClear(GL_COLOR_BUFFER_BIT);
            return;

        case BUFFER_DEPTH:
            glClear(GL_DEPTH_BUFFER_BIT);
            return;

        case BUFFER_STENCIL:
            glClear(GL_STENCIL_BUFFER_BIT);
            return;

        case BUFFER_COLOR | BUFFER_DEPTH:
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;

        case BUFFER_COLOR | BUFFER_STENCIL:
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            return;

        case BUFFER_COLOR | BUFFER_DEPTH | BUFFER_STENCIL:
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            return;

        case BUFFER_DEPTH | BUFFER_STENCIL:
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            return;
    }
}

void ES2Context::setClearColor(const MVector4 & color){
    glClearColor(color.x, color.y, color.z, color.w);
}

// texture
void ES2Context::enableTexture(void)
{
    glEnable(GL_TEXTURE_2D);
}

void ES2Context::disableTexture(void)
{
    glDisable(GL_TEXTURE_2D);
}

void ES2Context::setTextureGenMode(M_TEX_GEN_MODES mode)
{
}

void ES2Context::setTextureFilterMode(M_TEX_FILTER_MODES min, M_TEX_FILTER_MODES mag)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, returnTexFilterMode(min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, returnTexFilterMode(mag));
}

void ES2Context::setTextureUWrapMode(M_WRAP_MODES wrap)
{
    int glWrap = GL_REPEAT;
    if(wrap == M_WRAP_CLAMP)
        glWrap = GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
}

void ES2Context::setTextureVWrapMode(M_WRAP_MODES wrap)
{
    int glWrap = GL_REPEAT;
    if(wrap == M_WRAP_CLAMP)
        glWrap = GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
}

void ES2Context::setTextureCombineMode(M_TEX_COMBINE_MODES combine)
{
    /*
     *	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
     *
     *	switch(combine)
     *	{
     *        case M_TEX_COMBINE_REPLACE:
     *            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
     *            break;
     *
     *        case M_TEX_COMBINE_MODULATE:
     *            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
     *            break;
     *
     *        case M_TEX_COMBINE_ADD:
     *            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
     *            break;
     *
     *        case M_TEX_COMBINE_SUB:
     *            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_SUBTRACT);
     *            break;
     *
     *        default:
     *            break;
}*/
}

void ES2Context::bindTexture(unsigned int textureId, unsigned int multitextureId)
{
    glActiveTexture(GL_TEXTURE0 + multitextureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void ES2Context::createTexture(unsigned int * textureId)
{
    glGenTextures(1, textureId);
}

void ES2Context::deleteTexture(unsigned int * textureId)
{
    glDeleteTextures(1, textureId);
}

void ES2Context::sendTextureImage(MImage * image, bool mipMap, bool filter, bool compress)
{
    // get properties
    unsigned int bytePerPix = image->getComponents();

    unsigned int width  = image->getWidth();
    unsigned int height = image->getHeight();

	int internalFormat = GL_RGB;
	int format = GL_RGB;

    /*if(compress)
     *     {
     *     if(bytePerPix == 4)
     *     {
     *     format = GL_RGBA;
     *     internalFormat = GL_COMPRESSED_RGBA_ARB;
}
else
{
internalFormat = GL_COMPRESSED_RGB_ARB;
}
}
else*/
    {
        if(bytePerPix == 4)
        {
            format = GL_RGBA;
            internalFormat = GL_RGBA;
        }
    }

	int glType = GL_TEXTURE_2D;

	glEnable(glType);

	if(filter)
	{
		glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(mipMap)
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if(mipMap)
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	glTexImage2D(glType, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, image->getData());
	if(mipMap)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
		glGenerateMipmap(glType);
	}
}

void ES2Context::texImage(unsigned int level, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
    GLenum format = returnTexMode(mode);
    GLenum intFormat = format;

    glTexImage2D(GL_TEXTURE_2D, level, intFormat, width, height, 0, format, returnGLType(type), pixels);
    if(level > 0)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void ES2Context::texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
    GLenum format = returnTexMode(mode);
    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, returnGLType(type), pixels);
    if(level > 0)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void ES2Context::generateMipMap(void){
    glGenerateMipmap(GL_TEXTURE_2D);
}

// frame buffer
void ES2Context::createFrameBuffer(unsigned int * frameBufferId){
    glGenFramebuffers(1, frameBufferId);
}
void ES2Context::deleteFrameBuffer(unsigned int * frameBufferId){
#ifndef EMSCRIPTEN
    glDeleteFramebuffers(1, frameBufferId);
#else
    MLOG_INFO("Can't delete frame buffer on WebGL");
#endif
}
void ES2Context::bindFrameBuffer(unsigned int frameBufferId){
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
    m_currentFrameBuffer = frameBufferId;
}
void ES2Context::getCurrentFrameBuffer(unsigned int * frameBufferId){
    (*frameBufferId) = m_currentFrameBuffer;
}
void ES2Context::attachFrameBufferTexture(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId){
    glFramebufferTexture2D(GL_FRAMEBUFFER, returnAttachType(attachment), GL_TEXTURE_2D, textureId, 0);
}
void ES2Context::setDrawingBuffers(M_FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size)
{
    /*
     *     if(size == 0)
     *     {
     *		 glDrawBuffer(GL_NONE);
     *		 glReadBuffer(GL_NONE);
}
else
{
static GLenum glBuffers[8];

size = MIN(8, size);
for(unsigned int i=0; i<size; i++)
    glBuffers[i] = returnAttachType(buffers[i]);

glDrawBuffers(size, glBuffers);

glDrawBuffer(GL_BACK);
glReadBuffer(GL_BACK);
}*/
}

// shaders
void ES2Context::createVertexShader(unsigned int * shaderId){
    *shaderId = glCreateShader(GL_VERTEX_SHADER);
}

void ES2Context::createPixelShader(unsigned int * shaderId){
    *shaderId = glCreateShader(GL_FRAGMENT_SHADER);
}

void ES2Context::deleteShader(unsigned int * shaderId){
    glDeleteShader((*shaderId));
}

bool ES2Context::sendShaderSource(unsigned int shaderId, const char * source)
{
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);

    GLint compiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        MLOG_ERROR("OpenGL : unable to compile shader\n");
        char shader_link_error[4096];
        glGetShaderInfoLog(shaderId, sizeof(shader_link_error), NULL, shader_link_error);
        MLOG_ERROR(shader_link_error);
		MLOG_ERROR("Sources: \n" << source);
        return false;
    }
    return true;
}

// FX
void ES2Context::bindFX(unsigned int fxId){
    glUseProgram(fxId);
}

void ES2Context::createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId)
{
    *fxId = glCreateProgram();
    glAttachShader(*fxId, vertexShaderId);
    glAttachShader(*fxId, pixelShaderId);
    glLinkProgram(*fxId);
}

void ES2Context::updateFX(unsigned int fxId)
{
    glLinkProgram(fxId);
}

void ES2Context::deleteFX(unsigned int * fxId){
    glDeleteProgram(*fxId);
}

void ES2Context::sendUniformInt(unsigned int fxId, const char * name, int * values, const int count){
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniform1iv(uValue, count, values);
}

void ES2Context::sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count){
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniform1fv(uValue, count, values);
}

void ES2Context::sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count){
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniform2fv(uValue, count, values);
}

void ES2Context::sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count){
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniform3fv(uValue, count, values);
}

void ES2Context::sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count){
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniform4fv(uValue, count, values);
}

void ES2Context::sendUniformMatrix(unsigned int fxId, const char * name, Matrix4x4 * matrix, const int count, const bool transpose){
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniformMatrix4fv(uValue, count, transpose, matrix->entries);
}

void ES2Context::getAttribLocation(unsigned int fxId, const char * name, int * location)
{
    (*location) = glGetAttribLocation(fxId, name);
}

// arrays
void ES2Context::enableVertexArray(void)
{
    //int location;
    //bindFX(simpleFX);
    //getAttribLocation(simpleFX, "Vertex", &location);
    //enableAttribArray(location);
}

void ES2Context::enableColorArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Color", &location);
    //enableAttribArray(location);
}

void ES2Context::enableNormalArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Normal", &location);
    //enableAttribArray(location);
}

void ES2Context::enableTexCoordArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "TexCoord", &location);
    //enableAttribArray(location);
}

void ES2Context::enableAttribArray(unsigned int location)
{
    glEnableVertexAttribArray(location);
}

void ES2Context::disableVertexArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Vertex", &location);
    //disableAttribArray(location);
}

void ES2Context::disableColorArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Color", &location);
    //disableAttribArray(location);
}

void ES2Context::disableNormalArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Normal", &location);
    //disableAttribArray(location);
}

void ES2Context::disableTexCoordArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "TexCoord", &location);
    //disableAttribArray(location);
}

void ES2Context::disableAttribArray(unsigned int location)
{
    glDisableVertexAttribArray(location);
}

void ES2Context::setVertexPointer(M_TYPES type, unsigned int components, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "Vertex", &location);
    //setAttribPointer(location, type, components, pointer);
}

void ES2Context::setColorPointer(M_TYPES type, unsigned int components, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "Color", &location);
    //setAttribPointer(location, type, components, pointer);
}

void ES2Context::setNormalPointer(M_TYPES type, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "Normal", &location);
    //setAttribPointer(location, type, components, pointer, 1);
}

void ES2Context::setTexCoordPointer(M_TYPES type, unsigned int components, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "TexCoord", &location);
    //setAttribPointer(location, type, components, pointer);
}

void ES2Context::setAttribPointer(unsigned int location, M_TYPES type, unsigned int components, const void * pointer, const bool normalized)
{
    glVertexAttribPointer(location, components, returnGLType(type), normalized, 0, pointer);
}

// draw
void ES2Context::drawArray(M_PRIMITIVE_TYPES type, unsigned int begin, unsigned int size)
{
    glDrawArrays(returnPrimitiveType(type), begin, size);
}

void ES2Context::drawElement(M_PRIMITIVE_TYPES type, unsigned int size, M_TYPES indicesType, const void * indices)
{
    glDrawElements(returnPrimitiveType(type), size, returnGLType(indicesType), indices);
}

// lines
void ES2Context::enableLineAntialiasing(void)	{ /*glEnable(GL_LINE_SMOOTH);*/ }
void ES2Context::disableLineAntialiasing(void)	{ /*glDisable(GL_LINE_SMOOTH);*/ }

// material
void ES2Context::setMaterialDiffuse(const MVector4 & diffuse){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}
void ES2Context::setMaterialSpecular(const MVector4 & specular){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}
void ES2Context::setMaterialAmbient(const MVector4 & ambient){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void ES2Context::setMaterialEmit(const MVector4 & emit){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
}
void ES2Context::setMaterialShininess(float shininess){
    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// scissor
void ES2Context::enableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }
void ES2Context::disableScissorTest(void){ glDisable(GL_SCISSOR_TEST); }
void ES2Context::setScissor(int x, int y, unsigned int width, unsigned int height){
    glScissor(x, y, width, height);
}

// color
void ES2Context::setColor(const MColor & color)
{
    //glColor4ub(color.r, color.g, color.b, color.a);
}

void ES2Context::setColor4(const MVector4 & color)
{
    //glColor4f(color.x, color.y, color.z, color.w);
}
void ES2Context::setColor3(const MVector3 & color)
{
    //glColor4f(color.x, color.y, color.z, 1.0f);
}

// masks
void ES2Context::setColorMask(bool r, bool g, bool b, bool a){
    glColorMask(r, g, b, a);
}
void ES2Context::setDepthMask(bool depth){
    glDepthMask(depth);
}

void ES2Context::setAlphaTest(float value)
{
    /*
     *	if(value > 0.0f)
     *		glEnable(GL_ALPHA_TEST);
     *	else {
     *		glDisable(GL_ALPHA_TEST);
}

glAlphaFunc(GL_GREATER, value);*/
}

// depth
void ES2Context::enableDepthTest(void) { glEnable (GL_DEPTH_TEST); }
void ES2Context::disableDepthTest(void){ glDisable(GL_DEPTH_TEST); }
void ES2Context::setDepthMode(M_DEPTH_MODES mode)
{
    switch(mode)
    {
        case M_DEPTH_ALWAYS:
            glDepthFunc(GL_ALWAYS);
            return;

        case M_DEPTH_LESS:
            glDepthFunc(GL_LESS);
            return;

        case M_DEPTH_GREATER:
            glDepthFunc(GL_GREATER);
            return;

        case M_DEPTH_EQUAL:
            glDepthFunc(GL_EQUAL);
            return;

        case M_DEPTH_LEQUAL:
            glDepthFunc(GL_LEQUAL);
            return;

        case M_DEPTH_GEQUAL:
            glDepthFunc(GL_GEQUAL);
            return;

        case M_DEPTH_NOTEQUAL:
            glDepthFunc(GL_NOTEQUAL);
            return;

        default:
            break;
    }
}

// stencil
void ES2Context::enableStencilTest(void) { glEnable (GL_STENCIL_TEST); }
void ES2Context::disableStencilTest(void){ glDisable(GL_STENCIL_TEST); }

// cull face
void ES2Context::enableCullFace(void) { glEnable (GL_CULL_FACE); }
void ES2Context::disableCullFace(void){ glDisable(GL_CULL_FACE); }
void ES2Context::setCullMode(M_CULL_MODES mode)
{
    switch(mode)
    {
        case M_CULL_FRONT:
            glCullFace(GL_FRONT);
            return;

        case M_CULL_BACK:
            glCullFace(GL_BACK);
            return;

        case M_CULL_FRONT_BACK:
            glCullFace(GL_FRONT_AND_BACK);
            return;

        default:
            break;
    }
}

// matrix
void ES2Context::loadIdentity(void)
{
    m_currentMatrix->loadIdentity();
}

void ES2Context::setMatrixMode(M_MATRIX_MODES matrixMode)
{
    m_matrixStep = 0;
    m_matrixMode = matrixMode;

    switch(matrixMode)
    {
        case M_MATRIX_MODELVIEW:
            m_currentMatrix = &m_modelViewMatrix;
            return;

        case M_MATRIX_PROJECTION:
            m_currentMatrix = &m_projectionMatrix;
            return;

        case M_MATRIX_TEXTURE:
            m_currentMatrix = &m_textureMatrix;
            return;
    }
}

void ES2Context::pushMatrix(void)
{
    m_matrixSteps[m_matrixStep] = *m_currentMatrix;
    m_matrixStep++;
}
void ES2Context::popMatrix(void)
{
    if(m_matrixStep > 0)
    {
        m_matrixStep--;
        (*m_currentMatrix) = m_matrixSteps[m_matrixStep];
    }
}
void ES2Context::multMatrix(const Matrix4x4 * matrix){
    (*m_currentMatrix) *= (*matrix);
}
void ES2Context::translate(const MVector3 & position){
    m_currentMatrix->translate(position);
}
void ES2Context::rotate(const MVector3 & axis, float angle){
    m_currentMatrix->rotate(axis, angle);
}
void ES2Context::scale(const MVector3 & scale){
    m_currentMatrix->scale(scale);
}
void ES2Context::getViewport(int * viewport)
{
    glGetIntegerv(GL_VIEWPORT, viewport);
}
void ES2Context::getModelViewMatrix(Matrix4x4 * matrix)
{
    (*matrix) = m_modelViewMatrix;
}
void ES2Context::getProjectionMatrix(Matrix4x4 * matrix)
{
    (*matrix) = m_projectionMatrix;
}
void ES2Context::getTextureMatrix(Matrix4x4 * matrix)
{
    (*matrix) = m_textureMatrix;
}

// fog
void ES2Context::enableFog(void)						{ /*glEnable (GL_FOG);*/ }
void ES2Context::disableFog(void)						{ /*glDisable(GL_FOG);*/ }
void ES2Context::setFogColor(const MVector3 & color)
{
    //glFogfv(GL_FOG_COLOR, MVector4(color));
    m_fogColor = color;
}
void ES2Context::setFogDistance(float min, float max)
{
    //glFogf(GL_FOG_START, min);
    //glFogf(GL_FOG_END,   max);
    m_fogMin = min;
    m_fogMax = max;
}
void ES2Context::getFogColor(MVector3 * color){
    (*color) = m_fogColor;
}
void ES2Context::getFogDistance(float * min, float * max){
    (*min) = m_fogMin;
    (*max) = m_fogMax;
}

// lighting
void ES2Context::enableLighting(void)			{ /*glEnable (GL_LIGHTING);*/ }
void ES2Context::disableLighting(void)			{ /*glDisable(GL_LIGHTING);*/ }
void ES2Context::enableLight(unsigned int id)	{ /*glEnable (GL_LIGHT0 + id);*/ }
void ES2Context::disableLight(unsigned int id)	{ /*glDisable(GL_LIGHT0 + id);*/ }
void ES2Context::setLightPosition(unsigned int id, const MVector4 & position)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].position = position;
        //glLightfv(GL_LIGHT0 + id, GL_POSITION, position);
    }
}
void ES2Context::setLightDiffuse(unsigned int id, const MVector4 & diffuse)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].diffuse = diffuse;
        //glLightfv(GL_LIGHT0 + id, GL_DIFFUSE, diffuse);
    }
}
void ES2Context::setLightSpecular(unsigned int id, const MVector4 & specular)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].specular = specular;
        //glLightfv(GL_LIGHT0 + id, GL_SPECULAR, specular);
    }
}
void ES2Context::setLightAmbient(unsigned int id, const MVector4 & ambient)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].ambient = ambient;
        //glLightfv(GL_LIGHT0 + id, GL_AMBIENT, ambient);
    }
}
void ES2Context::setLightAttenuation(unsigned int id, float constant, float linear, float quadratic)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].constant = constant;
        m_lights[id].linear = linear;
        m_lights[id].quadratic = quadratic;
        //glLightf(GL_LIGHT0 + id, GL_CONSTANT_ATTENUATION, constant);
        //glLightf(GL_LIGHT0 + id, GL_LINEAR_ATTENUATION, linear);
        //glLightf(GL_LIGHT0 + id, GL_QUADRATIC_ATTENUATION, quadratic);
    }
}
void ES2Context::setLightSpotDirection(unsigned int id, const MVector3 & direction)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].direction = direction;
        //glLightfv(GL_LIGHT0 + id, GL_SPOT_DIRECTION, direction);
    }
}
void ES2Context::setLightSpotAngle(unsigned int id, float angle)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].angle = angle;
        //glLightf(GL_LIGHT0 + id, GL_SPOT_CUTOFF, angle);
    }
}
void ES2Context::setLightSpotExponent(unsigned int id, float exponent)
{
    if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].exponent = exponent*128.0f;
        //glLightf(GL_LIGHT0 + id, GL_SPOT_EXPONENT, m_lights[id].exponent);
    }
}
void ES2Context::getLightPosition(unsigned int id, MVector4 * position)
{
    if(id < MAX_MRCLIGHTS)
        (*position) = m_lights[id].position;
}
void ES2Context::getLightDiffuse(unsigned int id, MVector4 * diffuse)
{
    if(id < MAX_MRCLIGHTS)
        (*diffuse) = m_lights[id].diffuse;
}
void ES2Context::getLightSpecular(unsigned int id, MVector4 * specular)
{
    if(id < MAX_MRCLIGHTS)
        (*specular) = m_lights[id].specular;
}
void ES2Context::getLightAmbient(unsigned int id, MVector4 * ambient)
{
    if(id < MAX_MRCLIGHTS)
        (*ambient) = m_lights[id].ambient;
}
void ES2Context::getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic)
{
    if(id < MAX_MRCLIGHTS)
    {
        (*constant) = m_lights[id].constant;
        (*linear) = m_lights[id].linear;
        (*quadratic) = m_lights[id].quadratic;
    }
}
void ES2Context::getLightSpotDirection(unsigned int id, MVector3 * direction)
{
    if(id < MAX_MRCLIGHTS)
        (*direction) = m_lights[id].direction;
}
void ES2Context::getLightSpotAngle(unsigned int id, float * angle)
{
    if(id < MAX_MRCLIGHTS)
        (*angle) = m_lights[id].angle;
}
void ES2Context::getLightSpotExponent(unsigned int id, float * exponent)
{
    if(id < MAX_MRCLIGHTS)
        (*exponent) = m_lights[id].exponent;
}

// blending
void ES2Context::enableBlending(void)	{ glEnable (GL_BLEND); }
void ES2Context::disableBlending(void)	{ glDisable(GL_BLEND); }
void ES2Context::setBlendingMode(M_BLENDING_MODES mode)
{
    switch(mode)
    {
		case M_BLENDING_NONE:
            glBlendFunc(GL_ONE, GL_ZERO);
            break;

        case M_BLENDING_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case M_BLENDING_ADD:
            glBlendFunc(GL_ONE, GL_ONE);
            break;

        case M_BLENDING_PRODUCT:
            glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            break;

        case M_BLENDING_LIGHT:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            break;

        default:
			break;
    }
}

// point size
void ES2Context::setPointSize(float size)
{

}

#endif

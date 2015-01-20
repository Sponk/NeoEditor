/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// GLContext.cpp
//
// OpenGL-Glew Rendering Context
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

#ifndef USE_GLES
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// For linking Glew
#define GLEW_STATIC

#ifdef __APPLE__
	#include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
#elif !defined(EMSCRIPTEN)
    #include <glew.h>
#else
    #include <GL/glew.h>
    #include <GL/gl.h>
#endif

#include <NeoEngine.h>
#include "GLContext.h"

using namespace Neo;

static int g_GLversion = 0;
static float maxAnisotropy = 0.0f;

GLenum returnGLType(M_TYPES type)
{
	switch(type)
	{
	default:
		return GL_NONE;

	case M_BOOL:
		return GL_BOOL;

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

	case M_DOUBLE:
		return GL_DOUBLE;
	}
}

GLenum returnPrimitiveType(M_PRIMITIVE_TYPES type)
{
	switch(type)
	{
	default:
		return GL_NONE;

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

	case M_PRIMITIVE_TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;

    case M_PRIMITIVE_POINTS:
        return GL_POINTS;
	}
}

GLenum returnTexFilterMode(M_TEX_FILTER_MODES mode)
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

GLenum returnTexMode(M_TEX_MODES mode)
{
	switch(mode)
	{
	case M_R:
		return GL_R;

	case M_RG:
		return GL_RG;

	default:
	case M_RGB:
		return GL_RGB;

	case M_RGBA:
		return GL_RGBA;

	case M_DEPTH:
		return GL_DEPTH_COMPONENT;
	}
}

GLenum returnAttachType(M_FRAME_BUFFER_ATTACHMENT type)
{
	if(type == M_ATTACH_DEPTH)
		return GL_DEPTH_ATTACHMENT_EXT;
	else if(type == M_ATTACH_STENCIL)
		return GL_STENCIL_ATTACHMENT_EXT;
	else
		return GL_COLOR_ATTACHMENT0_EXT + ((int)type - 2);
}

GLContext::GLContext(void):
m_currentFrameBuffer(0)
{
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        MLOG_ERROR("Can't initialize GLEW: " << glewGetErrorString(err));
        return;
    }

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

	// normalize
	glEnable(GL_NORMALIZE);

	// fog
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_MODE, GL_LINEAR);

	// depth
	enableDepthTest();
	setDepthMode(M_DEPTH_LEQUAL);
	glClearDepth(1.0f);

	// line
	glLineWidth(1);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// point
    glPointSize(2.0);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_PROGRAM_POINT_SIZE_ARB);

    float coeffs[] = {2.0f, 1.0f, 0.1f};
    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, coeffs);

    glPointParameterf (GL_POINT_SIZE_MAX, 100.0);
    glPointParameterf (GL_POINT_SIZE_MIN, 0.1f);

	// stencil
	glClearStencil(0);

	// pixel pack/unpack
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

    //glPointSize(50);

	// anisotropic filtering
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
}

GLContext::~GLContext(void)
{}

// view
void GLContext::setPerspectiveView(float fov, float ratio, float zNear, float zFar)
{
	MMatrix4x4 matrix;

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

void GLContext::setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar){
	glOrtho(left, right, bottom, top, zNear, zFar);
}

// viewport
void GLContext::setViewport(int x, int y, unsigned int width, unsigned int height){
	glViewport(x, y, width, height);
}

// clear
void GLContext::clear(int buffer)
{
	switch(buffer)
	{
	case M_BUFFER_COLOR:
		glClear(GL_COLOR_BUFFER_BIT);
		return;

	case M_BUFFER_DEPTH:
		glClear(GL_DEPTH_BUFFER_BIT);
		return;

	case M_BUFFER_STENCIL:
		glClear(GL_STENCIL_BUFFER_BIT);
		return;

	case M_BUFFER_COLOR | M_BUFFER_DEPTH:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;

	case M_BUFFER_COLOR | M_BUFFER_STENCIL:
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return;

	case M_BUFFER_COLOR | M_BUFFER_DEPTH | M_BUFFER_STENCIL:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return;

	case M_BUFFER_DEPTH | M_BUFFER_STENCIL:
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return;
	}
}

void GLContext::setClearColor(const MVector4 & color){
	glClearColor(color.x, color.y, color.z, color.w);
}

// texture
void GLContext::enableTexture(void)
{
	glEnable(GL_TEXTURE_2D);
}

void GLContext::disableTexture(void)
{
	glDisable(GL_TEXTURE_2D);
}

void GLContext::setTextureGenMode(M_TEX_GEN_MODES mode)
{
}

void GLContext::setTextureFilterMode(M_TEX_FILTER_MODES min, M_TEX_FILTER_MODES mag)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, returnTexFilterMode(min));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, returnTexFilterMode(mag));
}

void GLContext::setTextureUWrapMode(M_WRAP_MODES wrap)
{
	int glWrap = GL_REPEAT;
	if(wrap == M_WRAP_CLAMP)
		glWrap = GL_CLAMP_TO_EDGE;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
}

void GLContext::setTextureVWrapMode(M_WRAP_MODES wrap)
{
	int glWrap = GL_REPEAT;
	if(wrap == M_WRAP_CLAMP)
		glWrap = GL_CLAMP_TO_EDGE;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
}

void GLContext::setTextureCombineMode(M_TEX_COMBINE_MODES combine)
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

	switch(combine)
	{
	case M_TEX_COMBINE_REPLACE:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
		break;

	case M_TEX_COMBINE_MODULATE:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
		break;

	case M_TEX_COMBINE_ADD:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD);
		break;

	case M_TEX_COMBINE_SUB:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_SUBTRACT);
		break;
	}
}

void GLContext::bindTexture(unsigned int textureId, unsigned int multitextureId)
{
	glActiveTexture(GL_TEXTURE0 + multitextureId);
	glClientActiveTexture(GL_TEXTURE0 + multitextureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLContext::createTexture(unsigned int * textureId)
{
	glGenTextures(1, textureId);
}

void GLContext::deleteTexture(unsigned int * textureId)
{
	glDeleteTextures(1, textureId);
}

void GLContext::sendTextureImage(MImage * image, bool mipMap, bool filter, bool compress)
{
	// get properties
	unsigned int bytePerPix = image->getComponents();

	unsigned int width  = image->getWidth();
	unsigned int height = image->getHeight();

	int internalFormat = GL_RGB;
	int format = GL_RGB;

	if(compress)
	{
		if(bytePerPix == 4)
		{
			format = GL_RGBA;
			internalFormat = GL_COMPRESSED_RGBA_ARB;
		}
		else
		{
			internalFormat = GL_COMPRESSED_RGB_ARB;
		}
	}
	else
	{
		if(bytePerPix == 4)
		{
			format = GL_RGBA;
			internalFormat = GL_RGBA;
		}
	}

	int glType = GL_TEXTURE_2D;

	glEnable(glType);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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
		glGenerateMipmapEXT(glType);
	}

	/*
	if(mipMap)
		gluBuild2DMipmaps(glType, bytePerPix, width, height, format, GL_UNSIGNED_BYTE, image->getData());
	else
		glTexImage2D(glType, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, image->getData());
	*/
}

void GLContext::texImage(unsigned int level, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
	GLenum format = returnTexMode(mode);
	GLenum intFormat = format;
	//if(type == M_FLOAT && mode == M_RGB)
	//	intFormat = GL_RGB32F_ARB;

	glTexImage2D(GL_TEXTURE_2D, level, intFormat, width, height, 0, format, returnGLType(type), pixels);
	if(level > 0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void GLContext::texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
	GLenum format = returnTexMode(mode);
	glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, returnGLType(type), pixels);
	if(level > 0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void GLContext::generateMipMap(void){
	glGenerateMipmapEXT(GL_TEXTURE_2D);
}

void GLContext::getTexImage(unsigned int level, MImage * image)
{
	if(image)
	{
		int width, height;
		int r, g, b, a, depth, format;

		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_RED_SIZE, &r);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_GREEN_SIZE, &g);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_BLUE_SIZE, &b);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_ALPHA_SIZE, &a);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_DEPTH_SIZE, &depth);

		unsigned int dpp = 0;
		if(b > 0)
		{
			if(a > 0)
			{
				format = GL_RGBA;
				dpp = 4;
			}
			else
			{
				format = GL_RGB;
				dpp = 3;
			}
		}
		else if(depth > 0)
		{
			format = GL_DEPTH_COMPONENT;
			dpp = 1;
		}

		if(dpp > 0)
		{
			image->create(M_UBYTE, width, height, dpp);
			glGetTexImage(GL_TEXTURE_2D, level, format, GL_UNSIGNED_BYTE, image->getData());
		}
	}
}

// frame buffer
void CHECK_FRAMEBUFFER_STATUS()
{
	GLenum status;
	status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	switch(status){
		case GL_FRAMEBUFFER_COMPLETE:
            break;
          case GL_FRAMEBUFFER_UNSUPPORTED:
			fprintf(stderr, "couldn't find a supported config\n");
			break;
          default:
			fprintf(stderr, "error");
	}
}


void GLContext::createFrameBuffer(unsigned int * frameBufferId){
	glGenFramebuffersEXT(1, frameBufferId);
}
void GLContext::deleteFrameBuffer(unsigned int * frameBufferId){
	glDeleteFramebuffersEXT(1, frameBufferId);
}
void GLContext::bindFrameBuffer(unsigned int frameBufferId){
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferId);
	m_currentFrameBuffer = frameBufferId;
}
void GLContext::getCurrentFrameBuffer(unsigned int * frameBufferId){
	(*frameBufferId) = m_currentFrameBuffer;
}
void GLContext::attachFrameBufferTexture(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId){
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, returnAttachType(attachment), GL_TEXTURE_2D, textureId, 0);
}
void GLContext::attachFrameBufferRB(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int renderBufferId){
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, returnAttachType(attachment), GL_RENDERBUFFER_EXT, renderBufferId);

	CHECK_FRAMEBUFFER_STATUS();
}
void GLContext::setDrawingBuffers(M_FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size)
{
	if(size == 0)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
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
	}
}

// render buffer
void GLContext::createRenderBuffer(unsigned int * renderBufferId){
	glGenRenderbuffersEXT(1, renderBufferId);
}

void GLContext::deleteRenderBuffer(unsigned int * renderBufferId){
	glDeleteRenderbuffersEXT(1, renderBufferId);
}

void GLContext::bindRenderBuffer(unsigned int renderBufferId){
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderBufferId);
}

void GLContext::setRenderBuffer(M_RENDER_BUFFER_MODES mode, unsigned int width, unsigned int height)
{
	GLenum internalMode;

	switch(mode)
	{
		default:
			return;
		case M_RENDER_DEPTH:
			internalMode = GL_DEPTH_COMPONENT;
			break;
		case M_RENDER_STENCIL:
			internalMode = GL_STENCIL_INDEX;
			break;
		case M_RENDER_DEPTH_STENCIL:
			internalMode = GL_DEPTH_STENCIL_EXT;
			break;
	}

	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, width, height);
}

// shaders
void GLContext::createVertexShader(unsigned int * shaderId){
	*shaderId = (unsigned int)(unsigned long)glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
}

void GLContext::createPixelShader(unsigned int * shaderId){
	*shaderId = (unsigned int)(unsigned long)glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
}

void GLContext::deleteShader(unsigned int * shaderId){
	glDeleteObjectARB((GLhandleARB)(*shaderId));
}

bool GLContext::sendShaderSource(unsigned int shaderId, const char * source)
{
	glShaderSourceARB((GLhandleARB)shaderId, 1, &source, NULL);
	glCompileShaderARB((GLhandleARB)shaderId);

	GLint compiled;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
	{
        MLOG_ERROR("OpenGL : Unable to compile shader");
		char shader_link_error[4096];
        glGetInfoLogARB((GLhandleARB)shaderId, sizeof(shader_link_error), NULL, shader_link_error);
        MLOG_ERROR(shader_link_error);
        return false;
	}

    return true;
}

// FX
void GLContext::bindFX(unsigned int fxId){
	glUseProgramObjectARB((GLhandleARB)fxId);
}

void GLContext::createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId)
{
	*fxId = (unsigned int)(unsigned long)glCreateProgramObjectARB();
	glAttachObjectARB((GLhandleARB)*fxId, (GLhandleARB)vertexShaderId);
	glAttachObjectARB((GLhandleARB)*fxId, (GLhandleARB)pixelShaderId);
	glLinkProgramARB((GLhandleARB)*fxId);
}

void GLContext::updateFX(unsigned int fxId)
{
	glLinkProgramARB((GLhandleARB)fxId);
}

void GLContext::deleteFX(unsigned int * fxId){
	glDeleteObjectARB((GLhandleARB)(*fxId));
}

void GLContext::sendUniformInt(unsigned int fxId, const char * name, int * values, const int count){
	GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	if(uValue != -1) glUniform1ivARB(uValue, count, values);
}

void GLContext::sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	if(uValue != -1) glUniform1fvARB(uValue, count, values);
}

void GLContext::sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	if(uValue != -1) glUniform2fvARB(uValue, count, values);
}

void GLContext::sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	if(uValue != -1) glUniform3fvARB(uValue, count, values);
}

void GLContext::sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	if(uValue != -1) glUniform4fvARB(uValue, count, values);
}

void GLContext::sendUniformMatrix(unsigned int fxId, const char * name, MMatrix4x4 * matrix, const int count, const bool transpose){
	GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	if(uValue != -1) glUniformMatrix4fvARB(uValue, count, transpose, matrix->entries);
}

void GLContext::getAttribLocation(unsigned int fxId, const char * name, int * location)
{
	(*location) = glGetAttribLocationARB((GLhandleARB)fxId, name);
}

// VBO
void GLContext::createVBO(unsigned int * vboId){
	glGenBuffersARB(1, vboId);
}
void GLContext::deleteVBO(unsigned int * vboId){
	glDeleteBuffersARB(1, vboId);
}
void GLContext::bindVBO(M_VBO_TYPES type, unsigned int vboId)
{
	GLenum gltype = type == M_VBO_ARRAY ? GL_ARRAY_BUFFER_ARB : GL_ELEMENT_ARRAY_BUFFER_ARB;
	glBindBufferARB(gltype, vboId);
}
void GLContext::setVBO(M_VBO_TYPES type, const void * data, unsigned int size, M_VBO_MODES mode)
{
	GLenum gltype = type == M_VBO_ARRAY ? GL_ARRAY_BUFFER_ARB : GL_ELEMENT_ARRAY_BUFFER_ARB;
	switch(mode)
	{
		case M_VBO_STATIC:
			glBufferDataARB(gltype, size, data, GL_STATIC_DRAW_ARB);
			break;
		case M_VBO_DYNAMIC:
			glBufferDataARB(gltype, size, data, GL_DYNAMIC_DRAW_ARB);
			break;
		case M_VBO_STREAM:
			glBufferDataARB(gltype, size, data, GL_STREAM_DRAW_ARB);
			break;
	}
}
void GLContext::setVBOSubData(M_VBO_TYPES type, unsigned int offset, const void * data, unsigned int size)
{
	GLenum gltype = type == M_VBO_ARRAY ? GL_ARRAY_BUFFER_ARB : GL_ELEMENT_ARRAY_BUFFER_ARB;
	glBufferSubDataARB(gltype, offset, size, data);
}

// arrays
void GLContext::enableVertexArray(void){
	glEnableClientState(GL_VERTEX_ARRAY);
}

void GLContext::enableColorArray(void){
	glEnableClientState(GL_COLOR_ARRAY);
}

void GLContext::enableNormalArray(void){
	glEnableClientState(GL_NORMAL_ARRAY);
}

void GLContext::enableTexCoordArray(void){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GLContext::enableAttribArray(unsigned int location)
{
	glEnableVertexAttribArray(location);
}

void GLContext::disableVertexArray(void){
	glDisableClientState(GL_VERTEX_ARRAY);
}

void GLContext::disableColorArray(void){
	glDisableClientState(GL_COLOR_ARRAY);
}

void GLContext::disableNormalArray(void){
	glDisableClientState(GL_NORMAL_ARRAY);
}

void GLContext::disableTexCoordArray(void){
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GLContext::disableAttribArray(unsigned int location)
{
	glDisableVertexAttribArray(location);
}

void GLContext::setVertexPointer(M_TYPES type, unsigned int components, const void * pointer){
	glVertexPointer(components, returnGLType(type), 0, pointer);
}

void GLContext::setColorPointer(M_TYPES type, unsigned int components, const void * pointer){
	glColorPointer(components, returnGLType(type), 0, pointer);
}

void GLContext::setNormalPointer(M_TYPES type, const void * pointer){
	glNormalPointer(returnGLType(type), 0, pointer);
}

void GLContext::setTexCoordPointer(M_TYPES type, unsigned int components, const void * pointer){
	glTexCoordPointer(components, returnGLType(type), 0, pointer);
}

void GLContext::setAttribPointer(unsigned int location, M_TYPES type, unsigned int components, const void * pointer, const bool normalized)
{
	glVertexAttribPointer(location, components, returnGLType(type), normalized, 0, pointer);
}

// draw
void GLContext::drawArray(M_PRIMITIVE_TYPES type, unsigned int begin, unsigned int size)
{
	glDrawArrays(returnPrimitiveType(type), begin, size);
}

void GLContext::drawElement(M_PRIMITIVE_TYPES type, unsigned int size, M_TYPES indicesType, const void * indices)
{
	glDrawElements(returnPrimitiveType(type), size, returnGLType(indicesType), indices);
}

// lines
void GLContext::enableLineAntialiasing(void)	{ glEnable(GL_LINE_SMOOTH); }
void GLContext::disableLineAntialiasing(void)	{ glDisable(GL_LINE_SMOOTH); }

// material
void GLContext::setMaterialDiffuse(const MVector4 & diffuse){
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}
void GLContext::setMaterialSpecular(const MVector4 & specular){
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}
void GLContext::setMaterialAmbient(const MVector4 & ambient){
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void GLContext::setMaterialEmit(const MVector4 & emit){
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
}
void GLContext::setMaterialShininess(float shininess){
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// scissor
void GLContext::enableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }
void GLContext::disableScissorTest(void){ glDisable(GL_SCISSOR_TEST); }
void GLContext::setScissor(int x, int y, unsigned int width, unsigned int height){
	glScissor(x, y, width, height);
}

// color
void GLContext::setColor(const MColor & color){
	glColor4ub(color.r, color.g, color.b, color.a);
}

void GLContext::setColor4(const MVector4 & color){
	glColor4f(color.x, color.y, color.z, color.w);
}
void GLContext::setColor3(const MVector3 & color){
	glColor4f(color.x, color.y, color.z, 1.0f);
}

// masks
void GLContext::setColorMask(bool r, bool g, bool b, bool a){
	glColorMask(r, g, b, a);
}
void GLContext::setDepthMask(bool depth){
	glDepthMask(depth);
}

void GLContext::enablePolygonOffset(float x, float y)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(x, y);
}

void GLContext::disablePolygonOffset()
{
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void GLContext::setAlphaTest(float value)
{
	if(value > 0.0f)
		glEnable(GL_ALPHA_TEST);
	else {
		glDisable(GL_ALPHA_TEST);
	}

	glAlphaFunc(GL_GREATER, value);
}

// depth
void GLContext::enableDepthTest(void) { glEnable (GL_DEPTH_TEST); }
void GLContext::disableDepthTest(void){ glDisable(GL_DEPTH_TEST); }
void GLContext::setDepthMode(M_DEPTH_MODES mode)
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
	}
}

// stencil
void GLContext::enableStencilTest(void) { glEnable (GL_STENCIL_TEST); }
void GLContext::disableStencilTest(void){ glDisable(GL_STENCIL_TEST); }
void GLContext::setStencilFunc(M_STENCIL_FUNCS func, int ref)
{
	switch(func)
	{
		default:
		case M_STENCIL_ALWAYS:
			glStencilFunc(GL_ALWAYS, ref, ~0);
			break;
		case M_STENCIL_EQUAL:
			glStencilFunc(GL_EQUAL, ref, ~0);
			break;
		case M_STENCIL_NOTEQUAL:
			glStencilFunc(GL_NOTEQUAL, ref, ~0);
			break;
		case M_STENCIL_NEVER:
			glStencilFunc(GL_NEVER, ref, ~0);
			break;
		case M_STENCIL_LESS:
			glStencilFunc(GL_LESS, ref, ~0);
			break;
		case M_STENCIL_LEQUAL:
			glStencilFunc(GL_LEQUAL, ref, ~0);
			break;
		case M_STENCIL_GREATER:
			glStencilFunc(GL_GREATER, ref, ~0);
			break;
		case M_STENCIL_GEQUAL:
			glStencilFunc(GL_GEQUAL, ref, ~0);
			break;
	}
}
void GLContext::setStencilOp(M_STENCIL_OPS op)
{
	switch(op)
	{
		default:
		case M_STENCIL_KEEP:
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			break;
		case M_STENCIL_INVERT:
			glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
			break;
		case M_STENCIL_DECR:
			glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
			break;
		case M_STENCIL_INCR:
			glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
			break;
		case M_STENCIL_REPLACE:
			glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
			break;
	}
}

// cull face
void GLContext::enableCullFace(void) { glEnable (GL_CULL_FACE); }
void GLContext::disableCullFace(void){ glDisable(GL_CULL_FACE); }
void GLContext::setCullMode(M_CULL_MODES mode)
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
	}
}

// occlusion
void GLContext::createQuery(unsigned int * queryId){
	glGenQueriesARB(1, queryId);
}
void GLContext::deleteQuery(unsigned int * queryId){
	glDeleteQueriesARB(1, queryId);
}
void GLContext::beginQuery(unsigned int queryId){
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queryId);
}
void GLContext::endQuery(void){
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
}
void GLContext::getQueryResult(unsigned int queryId, unsigned int * result){
	glGetQueryObjectuivARB(queryId, GL_QUERY_RESULT_ARB, result);
}

// matrix
void GLContext::loadIdentity(void){
	glLoadIdentity();
}

void GLContext::setMatrixMode(M_MATRIX_MODES matrixMode)
{
	switch(matrixMode)
	{
	case M_MATRIX_MODELVIEW:
		glMatrixMode(GL_MODELVIEW);
		return;

	case M_MATRIX_PROJECTION:
		glMatrixMode(GL_PROJECTION);
		return;

	case M_MATRIX_TEXTURE:
		glMatrixMode(GL_TEXTURE);
		return;
	}
}

void GLContext::pushMatrix(void)									{ glPushMatrix(); }
void GLContext::popMatrix(void)									{ glPopMatrix(); }
void GLContext::multMatrix(const MMatrix4x4 * matrix)				{ glMultMatrixf(matrix->entries); }
void GLContext::translate(const MVector3 & position)				{ glTranslatef(position.x, position.y, position.z); }
void GLContext::rotate(const MVector3 & axis, float angle)			{ glRotatef(angle, axis.x, axis.y, axis.z); }
void GLContext::scale(const MVector3 & scale)						{ glScalef(scale.x, scale.y, scale.z); }
void GLContext::getViewport(int * viewport)
{
	glGetIntegerv(GL_VIEWPORT, viewport);
}
void GLContext::getModelViewMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix->entries);
}
void GLContext::getProjectionMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_PROJECTION_MATRIX, matrix->entries);
}
void GLContext::getTextureMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_TEXTURE_MATRIX, matrix->entries);
}

// fog
void GLContext::enableFog(void)						{ glEnable (GL_FOG); }
void GLContext::disableFog(void)						{ glDisable(GL_FOG); }
void GLContext::setFogColor(const MVector3 & color)
{
	glFogfv(GL_FOG_COLOR, MVector4(color));
	m_fogColor = color;
}
void GLContext::setFogDistance(float min, float max)
{
	glFogf(GL_FOG_START, min);
	glFogf(GL_FOG_END,   max);
	m_fogMin = min;
	m_fogMax = max;
}
void GLContext::getFogColor(MVector3 * color){
	(*color) = m_fogColor;
}
void GLContext::getFogDistance(float * min, float * max){
	(*min) = m_fogMin;
	(*max) = m_fogMax;
}

// lighting
void GLContext::enableLighting(void)			{ glEnable (GL_LIGHTING); }
void GLContext::disableLighting(void)			{ glDisable(GL_LIGHTING); }
void GLContext::enableLight(unsigned int id)	{ glEnable (GL_LIGHT0 + id); }
void GLContext::disableLight(unsigned int id)	{ glDisable(GL_LIGHT0 + id); }
void GLContext::setLightPosition(unsigned int id, const MVector4 & position)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].position = position;
		glLightfv(GL_LIGHT0 + id, GL_POSITION, position);
	}
}
void GLContext::setLightDiffuse(unsigned int id, const MVector4 & diffuse)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].diffuse = diffuse;
		glLightfv(GL_LIGHT0 + id, GL_DIFFUSE, diffuse);
	}
}
void GLContext::setLightSpecular(unsigned int id, const MVector4 & specular)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].specular = specular;
		glLightfv(GL_LIGHT0 + id, GL_SPECULAR, specular);
	}
}
void GLContext::setLightAmbient(unsigned int id, const MVector4 & ambient)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].ambient = ambient;
		glLightfv(GL_LIGHT0 + id, GL_AMBIENT, ambient);
	}
}
void GLContext::setLightAttenuation(unsigned int id, float constant, float linear, float quadratic)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].constant = constant;
		m_lights[id].linear = linear;
		m_lights[id].quadratic = quadratic;
		glLightf(GL_LIGHT0 + id, GL_CONSTANT_ATTENUATION, constant);
		glLightf(GL_LIGHT0 + id, GL_LINEAR_ATTENUATION, linear);
		glLightf(GL_LIGHT0 + id, GL_QUADRATIC_ATTENUATION, quadratic);
	}
}
void GLContext::setLightSpotDirection(unsigned int id, const MVector3 & direction)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].direction = direction;
		glLightfv(GL_LIGHT0 + id, GL_SPOT_DIRECTION, direction);
	}
}
void GLContext::setLightSpotAngle(unsigned int id, float angle)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].angle = angle;
		glLightf(GL_LIGHT0 + id, GL_SPOT_CUTOFF, angle);
	}
}
void GLContext::setLightSpotExponent(unsigned int id, float exponent)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].exponent = exponent*128.0f;
		glLightf(GL_LIGHT0 + id, GL_SPOT_EXPONENT, m_lights[id].exponent);
	}
}
void GLContext::getLightPosition(unsigned int id, MVector4 * position)
{
	if(id < MAX_MRCLIGHTS)
		(*position) = m_lights[id].position;
}
void GLContext::getLightDiffuse(unsigned int id, MVector4 * diffuse)
{
	if(id < MAX_MRCLIGHTS)
		(*diffuse) = m_lights[id].diffuse;
}
void GLContext::getLightSpecular(unsigned int id, MVector4 * specular)
{
	if(id < MAX_MRCLIGHTS)
		(*specular) = m_lights[id].specular;
}
void GLContext::getLightAmbient(unsigned int id, MVector4 * ambient)
{
	if(id < MAX_MRCLIGHTS)
		(*ambient) = m_lights[id].ambient;
}
void GLContext::getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic)
{
	if(id < MAX_MRCLIGHTS)
	{
		(*constant) = m_lights[id].constant;
		(*linear) = m_lights[id].linear;
		(*quadratic) = m_lights[id].quadratic;
	}
}
void GLContext::getLightSpotDirection(unsigned int id, MVector3 * direction)
{
	if(id < MAX_MRCLIGHTS)
		(*direction) = m_lights[id].direction;
}
void GLContext::getLightSpotAngle(unsigned int id, float * angle)
{
	if(id < MAX_MRCLIGHTS)
		(*angle) = m_lights[id].angle;
}
void GLContext::getLightSpotExponent(unsigned int id, float * exponent)
{
	if(id < MAX_MRCLIGHTS)
		(*exponent) = m_lights[id].exponent;
}

// blending
void GLContext::enableBlending(void)	{ glEnable (GL_BLEND); }
void GLContext::disableBlending(void)	{ glDisable(GL_BLEND); }
void GLContext::setBlendingMode(M_BLENDING_MODES mode)
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
	}
}

// point size
void GLContext::setPointSize(float size)
{
    glPointSize(size);
}

#endif

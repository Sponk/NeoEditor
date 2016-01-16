//========================================================================
// Copyright (c) 2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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
	#include <glew.h>
	#include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
#elif !defined(EMSCRIPTEN)
    #include <glew.h>
#else
    #include <GL/glew.h>
    #include <GL/gl.h>
#endif

#include <NeoEngine.h>
#include "GL4Context.h"

using namespace Neo;

static int g_GLversion = 0;
static float maxAnisotropy = 0.0f;

static GLenum returnGLType(VAR_TYPES type)
{
    switch(type)
    {
        default:

            //case M_BOOL:
            //return GL_BOOL;

		case VAR_BYTE:
            return GL_BYTE;

		case VAR_UBYTE:
            return GL_UNSIGNED_BYTE;

		case VAR_SHORT:
            return GL_SHORT;

		case VAR_USHORT:
            return GL_UNSIGNED_SHORT;

		case VAR_INT:
            return GL_INT;

		case VAR_UINT:
            return GL_UNSIGNED_INT;

		case VAR_FLOAT:
            return GL_FLOAT;

            //case M_DOUBLE:
            //return GL_DOUBLE;
    }
}

static GLenum returnPrimitiveType(PRIMITIVE_TYPES type)
{
    switch(type)
    {
        default:
		case PRIMITIVE_LINES:
            return GL_LINES;

		case PRIMITIVE_LINE_LOOP:
            return GL_LINE_LOOP;

		case PRIMITIVE_LINE_STRIP:
            return GL_LINE_STRIP;

		case PRIMITIVE_TRIANGLES:
            return GL_TRIANGLES;

		case PRIMITIVE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;

		case PRIMITIVE_POINTS:
            return GL_POINTS;
    }
}

static GLenum returnTexFilterMode(TEX_FILTER_MODES mode)
{
    switch(mode)
    {
        default:
		case TEX_FILTER_NEAREST:
            return GL_NEAREST;

		case TEX_FILTER_NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;

		case TEX_FILTER_NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;

		case TEX_FILTER_LINEAR:
            return GL_LINEAR;

		case TEX_FILTER_LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;

		case TEX_FILTER_LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
}

static GLenum returnTexMode(TEX_MODES mode)
{
    switch(mode)
    {
        //case M_R:
        //	return GL_R;

        //case M_RG:
        //	return GL_RG;

        default:
		case TEX_RGB:
            return GL_RGB;

		case TEX_RGBA:
            return GL_RGBA;

		case TEX_DEPTH:
            return GL_DEPTH_COMPONENT;
    }
}

static GLenum returnFloatTexMode(TEX_MODES mode)
{
	switch (mode)
	{
		//case M_R:
		//	return GL_R;

		//case M_RG:
		//	return GL_RG;

	default:
	case TEX_RGB:
		return GL_RGB32F;

	case TEX_RGBA:
		return GL_RGBA32F;

	case TEX_DEPTH:
		return GL_DEPTH_COMPONENT32F;
	}
}

static GLenum returnAttachType(FRAME_BUFFER_ATTACHMENT type)
{
	if(type == ATTACH_DEPTH)
        return GL_DEPTH_ATTACHMENT;
	else if(type == ATTACH_STENCIL)
        return GL_STENCIL_ATTACHMENT;
    else
        return GL_COLOR_ATTACHMENT0 + ((int)type - 2);
}

GL4Context::GL4Context():
m_currentFrameBuffer(0),
m_gl_version(NULL)
{
}

GL4Context::~GL4Context()
{
}

//#define GL_DEBUG
#undef GL_DEBUG
#ifdef GL_DEBUG
void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum
                            severity, GLsizei length, GLchar* message, void* userParam)
{

    MLOG_INFO("--------------------- BEGIN OPENGL DEBUG MESSAGE -------------------");

    int size = strlen(message);
    if(size > 0 && message[size-1] == '\n')
        message[size-1] = '\0';

    stringstream ss;

    ss << "\tType: ";

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        ss << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        ss << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        ss << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        ss << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        ss << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        ss << "OTHER";
        break;
    }
    ss << endl;

    ss << "\tMessage: "<< message << endl;

    ss << "\tID: " << id << endl;
    ss << "\tSeverity: ";

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        ss << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        ss << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        ss << "HIGH";
        break;
    }
    ss << endl << endl;

    MLOG_INFO(ss.str());
    MLOG_INFO("--------------------- END OPENGL DEBUG MESSAGE ---------------------");
}
#endif

void GL4Context::init()
{
    // Do not initialize twice!
    if(m_gl_version != NULL)
        return;

	MLOG_INFO("********************************************************************************");
	MLOG_INFO("Initializing OpenGL context");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MLOG_ERROR("Can't initialize GLEW: " << glewGetErrorString(err));
		return;
	}

#ifdef GL_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)openglCallbackFunction, NULL);

	GLuint unusedIds = 0;
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds,
		true);
#endif

	m_gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	MLOG_INFO("Renderer:\t\t\t" << m_gl_version << "\t");
	MLOG_INFO("Vendor:\t\t\t" << glGetString(GL_VENDOR) << "\t");
	MLOG_INFO("GLSL Version:\t\t\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\t");

	int data;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &data);
	MLOG_INFO("Number of render targets:\t" << data << "\t");

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &data);
	MLOG_INFO("Number of textures:\t\t" << data << "\t");

	// init cull face (back)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearDepth(1.0);
	glDepthRange(0.0, 1.0);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// line
	glLineWidth(1.2f);

	// stencil
	glClearStencil(0);

	// pixel pack/unpack
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// anisotropic filtering
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_anisotropy);

	MLOG_INFO("Maximal anisotropy level:\t" << m_anisotropy << "\t");
	MLOG_INFO("********************************************************************************");
}

// view
void GL4Context::setPerspectiveView(float fov, float ratio, float zNear, float zFar)
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

void GL4Context::setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar)
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
void GL4Context::setViewport(int x, int y, unsigned int width, unsigned int height){
    glViewport(x, y, width, height);
}

// clear
void GL4Context::clear(int buffer)
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

void GL4Context::setClearColor(const Vector4 & color){
    glClearColor(color.x, color.y, color.z, color.w);
}

// texture
void GL4Context::enableTexture(void)
{
    //glEnable(GL_TEXTURE_2D);
}

void GL4Context::disableTexture(void)
{
    //glDisable(GL_TEXTURE_2D);
}

void GL4Context::setTextureGenMode(TEX_GEN_MODES mode)
{
}

void GL4Context::setTextureFilterMode(TEX_FILTER_MODES min, TEX_FILTER_MODES mag)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, returnTexFilterMode(min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, returnTexFilterMode(mag));
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_anisotropy);
}

void GL4Context::setTextureUWrapMode(WRAP_MODES wrap)
{
    int glWrap = GL_REPEAT;
	if(wrap == WRAP_CLAMP)
        glWrap = GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
}

void GL4Context::setTextureVWrapMode(WRAP_MODES wrap)
{
    int glWrap = GL_REPEAT;
	if(wrap == WRAP_CLAMP)
        glWrap = GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
}

void GL4Context::setTextureCombineMode(TEX_COMBINE_MODES combine)
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

void GL4Context::bindTexture(unsigned int textureId, unsigned int multitextureId)
{
    glActiveTexture(GL_TEXTURE0 + multitextureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void GL4Context::createTexture(unsigned int * textureId)
{
    glGenTextures(1, textureId);
}

void GL4Context::deleteTexture(unsigned int * textureId)
{
    glDeleteTextures(1, textureId);
}

void GL4Context::sendTextureImage(Image * image, bool mipMap, bool filter, bool compress)
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

        //glEnable(glType);

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

void GL4Context::texImage(unsigned int level, unsigned int width, unsigned int height, VAR_TYPES type, TEX_MODES mode, const void * pixels)
{
    GLenum format = returnTexMode(mode);
    GLenum intFormat = format;

	if (type == VAR_FLOAT)
		intFormat = returnFloatTexMode(mode);

	if(mode == TEX_DEPTH)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, returnGLType(type), NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

		// Allows for directly using the shadow mapping capabilties available in OpenGL
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
			
		return;
	}

    glTexImage2D(GL_TEXTURE_2D, level, intFormat, width, height, 0, format, returnGLType(type), pixels);
    if(level > 0)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void GL4Context::texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, VAR_TYPES type, TEX_MODES mode, const void * pixels)
{
    GLenum format = returnTexMode(mode);
    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, returnGLType(type), pixels);
    if(level > 0)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void GL4Context::generateMipMap(void)
{
    glGenerateMipmap(GL_TEXTURE_2D);
}

// frame buffer
void GL4Context::createFrameBuffer(unsigned int * frameBufferId)
{
    glGenFramebuffers(1, frameBufferId);
}

void GL4Context::deleteFrameBuffer(unsigned int * frameBufferId)
{
#ifndef EMSCRIPTEN
    glDeleteFramebuffers(1, frameBufferId);
#else
    MLOG_INFO("Can't delete frame buffer on WebGL");
#endif
}

void GL4Context::bindFrameBuffer(unsigned int frameBufferId)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
    m_currentFrameBuffer = frameBufferId;
}

void GL4Context::getCurrentFrameBuffer(unsigned int * frameBufferId)
{
    (*frameBufferId) = m_currentFrameBuffer;
}

void GL4Context::attachFrameBufferTexture(FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, returnAttachType(attachment), GL_TEXTURE_2D, textureId, 0);
}

void GL4Context::setDrawingBuffers(FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size)
{
    if(size == 0)
    {
        //glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        //glDrawBuffer(GL_NONE);
        //glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);
    }
    else
    {
        static GLenum glBuffers[8];

        size = MIN(8, size);
        for(unsigned int i=0; i<size; i++)
            glBuffers[i] = returnAttachType(buffers[i]);

        glDrawBuffers(size, glBuffers);

        //glDrawBuffer(GL_BACK);
        //glReadBuffer(GL_BACK);
    }
}

// shaders
void GL4Context::createVertexShader(unsigned int * shaderId)
{
    *shaderId = glCreateShader(GL_VERTEX_SHADER);
}

void GL4Context::createPixelShader(unsigned int * shaderId)
{
    *shaderId = glCreateShader(GL_FRAGMENT_SHADER);
}

void GL4Context::deleteShader(unsigned int * shaderId)
{
    glDeleteShader((*shaderId));
}

bool GL4Context::sendShaderSource(unsigned int shaderId, const char * source)
{
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);

    GLint compiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
    if(compiled == GL_FALSE)
    {
        MLOG_ERROR("OpenGL : unable to compile shader\n");
        char shader_link_error[4096];
        glGetShaderInfoLog(shaderId, sizeof(shader_link_error), NULL, shader_link_error);
        MLOG_ERROR(shader_link_error);
				// MLOG_ERROR("Sources: \n" << source);
        return false;
    }
    return true;
}

// FX
void GL4Context::bindFX(unsigned int fxId)
{
    glUseProgram(fxId);
}

void GL4Context::createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId)
{
    *fxId = glCreateProgram();
    glAttachShader(*fxId, vertexShaderId);
    glAttachShader(*fxId, pixelShaderId);
    glLinkProgram(*fxId);

	GLint linked;
	glGetProgramiv(*fxId, GL_LINK_STATUS, &linked);
	if(linked == GL_FALSE)
	{
		MLOG_ERROR("OpenGL : unable to link shader\n");
		char shader_link_error[4096];
		glGetProgramInfoLog(*fxId, sizeof(shader_link_error), NULL, shader_link_error);
		MLOG_ERROR(shader_link_error);
	}
}

void GL4Context::updateFX(unsigned int fxId)
{
    glLinkProgram(fxId);
}

void GL4Context::deleteFX(unsigned int * fxId)
{
    glDeleteProgram(*fxId);
}

void GL4Context::sendUniformInt(unsigned int fxId, const char * name, int * values, const int count)
{
    GLint uValue = getUniformLocation(fxId, name);
    if(uValue != -1) glUniform1iv(uValue, count, values);
}

void GL4Context::sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count)
{
    GLint uValue = getUniformLocation(fxId, name);
    if(uValue != -1) glUniform1fv(uValue, count, values);
}

void GL4Context::sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count)
{
    GLint uValue = getUniformLocation(fxId, name);
    if(uValue != -1) glUniform2fv(uValue, count, values);
}

void GL4Context::sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count)
{
    GLint uValue = glGetUniformLocation(fxId, name);
    if(uValue != -1) glUniform3fv(uValue, count, values);
}

void GL4Context::sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count)
{
    GLint uValue = getUniformLocation(fxId, name);
    if(uValue != -1) glUniform4fv(uValue, count, values);
}

void GL4Context::sendUniformMatrix(unsigned int fxId, const char * name, Matrix4x4 * matrix, const int count, const bool transpose)
{
	GLint uValue = getUniformLocation(fxId, name);
	if(uValue != -1) glUniformMatrix4fv(uValue, count, transpose, matrix->entries);
}

void GL4Context::getAttribLocation(unsigned int fxId, const char * name, int * location)
{
    (*location) = glGetAttribLocation(fxId, name);
}

// arrays
void GL4Context::enableVertexArray(void)
{
    //int location;
    //bindFX(simpleFX);
    //getAttribLocation(simpleFX, "Vertex", &location);
    //enableAttribArray(location);
}

void GL4Context::enableColorArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Color", &location);
    //enableAttribArray(location);
}

void GL4Context::enableNormalArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Normal", &location);
    //enableAttribArray(location);
}

void GL4Context::enableTexCoordArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "TexCoord", &location);
    //enableAttribArray(location);
}

void GL4Context::enableAttribArray(unsigned int location)
{
    glEnableVertexAttribArray(location);
}

void GL4Context::disableVertexArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Vertex", &location);
    //disableAttribArray(location);
}

void GL4Context::disableColorArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Color", &location);
    //disableAttribArray(location);
}

void GL4Context::disableNormalArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "Normal", &location);
    //disableAttribArray(location);
}

void GL4Context::disableTexCoordArray(void)
{
    //int location;
    //getAttribLocation(simpleFX, "TexCoord", &location);
    //disableAttribArray(location);
}

void GL4Context::disableAttribArray(unsigned int location)
{
    glDisableVertexAttribArray(location);
}

void GL4Context::setVertexPointer(VAR_TYPES type, unsigned int components, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "Vertex", &location);
    //setAttribPointer(location, type, components, pointer);
}

void GL4Context::setColorPointer(VAR_TYPES type, unsigned int components, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "Color", &location);
    //setAttribPointer(location, type, components, pointer);
}

void GL4Context::setNormalPointer(VAR_TYPES type, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "Normal", &location);
    //setAttribPointer(location, type, components, pointer, 1);
}

void GL4Context::setTexCoordPointer(VAR_TYPES type, unsigned int components, const void * pointer)
{
    //int location;
    //getAttribLocation(simpleFX, "TexCoord", &location);
    //setAttribPointer(location, type, components, pointer);
}

void GL4Context::setAttribPointer(unsigned int location, VAR_TYPES type, unsigned int components, const void * pointer, const bool normalized)
{
    glVertexAttribPointer(location, components, returnGLType(type), normalized, 0, pointer);
}

// draw
void GL4Context::drawArray(PRIMITIVE_TYPES type, unsigned int begin, unsigned int size)
{
    glDrawArrays(returnPrimitiveType(type), begin, size);
}

void GL4Context::drawElement(PRIMITIVE_TYPES type, unsigned int size, VAR_TYPES indicesType, const void * indices)
{
    glDrawElements(returnPrimitiveType(type), size, returnGLType(indicesType), indices);
}

// lines
void GL4Context::enableLineAntialiasing(void)	{ /*glEnable(GL_LINE_SMOOTH);*/ }
void GL4Context::disableLineAntialiasing(void)	{ /*glDisable(GL_LINE_SMOOTH);*/ }

// material
void GL4Context::setMaterialDiffuse(const Vector4 & diffuse){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}
void GL4Context::setMaterialSpecular(const Vector4 & specular){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}
void GL4Context::setMaterialAmbient(const Vector4 & ambient){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void GL4Context::setMaterialEmit(const Vector4 & emit){
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
}
void GL4Context::setMaterialShininess(float shininess){
    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// scissor
void GL4Context::enableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }
void GL4Context::disableScissorTest(void){ glDisable(GL_SCISSOR_TEST); }
void GL4Context::setScissor(int x, int y, unsigned int width, unsigned int height){
    glScissor(x, y, width, height);
}

// color
void GL4Context::setColor(const Color & color)
{
    //glColor4ub(color.r, color.g, color.b, color.a);
}

void GL4Context::setColor4(const Vector4 & color)
{
    //glColor4f(color.x, color.y, color.z, color.w);
}
void GL4Context::setColor3(const Vector3 & color)
{
    //glColor4f(color.x, color.y, color.z, 1.0f);
}

// masks
void GL4Context::setColorMask(bool r, bool g, bool b, bool a){
    glColorMask(r, g, b, a);
}
void GL4Context::setDepthMask(bool depth){
    glDepthMask(depth);
}

void GL4Context::setAlphaTest(float value)
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
void GL4Context::enableDepthTest(void) { glEnable (GL_DEPTH_TEST); }
void GL4Context::disableDepthTest(void){ glDisable(GL_DEPTH_TEST); }
void GL4Context::setDepthMode(DEPTH_MODES mode)
{
    switch(mode)
    {
		case DEPTH_ALWAYS:
            glDepthFunc(GL_ALWAYS);
            return;

		case DEPTH_LESS:
            glDepthFunc(GL_LESS);
            return;

		case DEPTH_GREATER:
            glDepthFunc(GL_GREATER);
            return;

		case DEPTH_EQUAL:
            glDepthFunc(GL_EQUAL);
            return;

		case DEPTH_LEQUAL:
            glDepthFunc(GL_LEQUAL);
            return;

		case DEPTH_GEQUAL:
            glDepthFunc(GL_GEQUAL);
            return;

		case DEPTH_NOTEQUAL:
            glDepthFunc(GL_NOTEQUAL);
            return;

        default:
            break;
    }
}

// stencil
void GL4Context::enableStencilTest(void) { glEnable (GL_STENCIL_TEST); }
void GL4Context::disableStencilTest(void){ glDisable(GL_STENCIL_TEST); }

// cull face
void GL4Context::enableCullFace(void) { glEnable (GL_CULL_FACE); }
void GL4Context::disableCullFace(void){ glDisable(GL_CULL_FACE); }
void GL4Context::setCullMode(CULL_MODES mode)
{
    switch(mode)
    {
		case CULL_FRONT:
            glCullFace(GL_FRONT);
            return;

		case CULL_BACK:
            glCullFace(GL_BACK);
            return;

		case CULL_FRONT_BACK:
            glCullFace(GL_FRONT_AND_BACK);
            return;

        default:
            break;
    }
}

// matrix
void GL4Context::loadIdentity(void)
{
    m_currentMatrix->loadIdentity();
}

void GL4Context::setMatrixMode(MATRIX_MODES matrixMode)
{
    m_matrixStep = 0;
    m_matrixMode = matrixMode;

    switch(matrixMode)
    {
		case MATRIX_MODELVIEW:
            m_currentMatrix = &m_modelViewMatrix;
            return;

		case MATRIX_PROJECTION:
            m_currentMatrix = &m_projectionMatrix;
            return;

		case MATRIX_TEXTURE:
            m_currentMatrix = &m_textureMatrix;
            return;
    }
}

void GL4Context::pushMatrix(void)
{
    m_matrixSteps[m_matrixStep] = *m_currentMatrix;
    m_matrixStep++;
}
void GL4Context::popMatrix(void)
{
    if(m_matrixStep > 0)
    {
        m_matrixStep--;
        (*m_currentMatrix) = m_matrixSteps[m_matrixStep];
    }
}

void GL4Context::multMatrix(const Matrix4x4 * matrix)
{
    (*m_currentMatrix) *= (*matrix);
}

void GL4Context::translate(const Vector3 & position)
{
    m_currentMatrix->translate(position);
}

void GL4Context::rotate(const Vector3 & axis, float angle)
{
    m_currentMatrix->rotate(axis, angle);
}

void GL4Context::scale(const Vector3 & scale)
{
    m_currentMatrix->scale(scale);
}

void GL4Context::getViewport(int * viewport)
{
    glGetIntegerv(GL_VIEWPORT, viewport);
}

void GL4Context::getModelViewMatrix(Matrix4x4 * matrix)
{
    (*matrix) = m_modelViewMatrix;
}

void GL4Context::getProjectionMatrix(Matrix4x4 * matrix)
{
    (*matrix) = m_projectionMatrix;
}

void GL4Context::getTextureMatrix(Matrix4x4 * matrix)
{
    (*matrix) = m_textureMatrix;
}

// fog
void GL4Context::enableFog(void)						{ /*glEnable (GL_FOG);*/ }
void GL4Context::disableFog(void)						{ /*glDisable(GL_FOG);*/ }
void GL4Context::setFogColor(const Vector3 & color)
{
	//glFogfv(GL_FOG_COLOR, Vector4(color));
    m_fogColor = color;
}
void GL4Context::setFogDistance(float min, float max)
{
    //glFogf(GL_FOG_START, min);
    //glFogf(GL_FOG_END,   max);
    m_fogMin = min;
    m_fogMax = max;
}
void GL4Context::getFogColor(Vector3 * color){
    (*color) = m_fogColor;
}
void GL4Context::getFogDistance(float * min, float * max){
    (*min) = m_fogMin;
    (*max) = m_fogMax;
}

// lighting
void GL4Context::enableLighting(void)			{ /*glEnable (GL_LIGHTING);*/ }
void GL4Context::disableLighting(void)			{ /*glDisable(GL_LIGHTING);*/ }
void GL4Context::enableLight(unsigned int id)	{ /*glEnable (GL_LIGHT0 + id);*/ }
void GL4Context::disableLight(unsigned int id)	{ /*glDisable(GL_LIGHT0 + id);*/ }
void GL4Context::setLightPosition(unsigned int id, const Vector4 & position)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].position = position;
        //glLightfv(GL_LIGHT0 + id, GL_POSITION, position);
    }*/
}
void GL4Context::setLightDiffuse(unsigned int id, const Vector4 & diffuse)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].diffuse = diffuse;
        //glLightfv(GL_LIGHT0 + id, GL_DIFFUSE, diffuse);
    }*/
}
void GL4Context::setLightSpecular(unsigned int id, const Vector4 & specular)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].specular = specular;
        //glLightfv(GL_LIGHT0 + id, GL_SPECULAR, specular);
    }*/
}
void GL4Context::setLightAmbient(unsigned int id, const Vector4 & ambient)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].ambient = ambient;
        //glLightfv(GL_LIGHT0 + id, GL_AMBIENT, ambient);
    }*/
}
void GL4Context::setLightAttenuation(unsigned int id, float constant, float linear, float quadratic)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].constant = constant;
        m_lights[id].linear = linear;
        m_lights[id].quadratic = quadratic;
        //glLightf(GL_LIGHT0 + id, GL_CONSTANT_ATTENUATION, constant);
        //glLightf(GL_LIGHT0 + id, GL_LINEAR_ATTENUATION, linear);
        //glLightf(GL_LIGHT0 + id, GL_QUADRATIC_ATTENUATION, quadratic);
    }*/
}
void GL4Context::setLightSpotDirection(unsigned int id, const Vector3 & direction)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].direction = direction;
        //glLightfv(GL_LIGHT0 + id, GL_SPOT_DIRECTION, direction);
    }*/
}
void GL4Context::setLightSpotAngle(unsigned int id, float angle)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].angle = angle;
        //glLightf(GL_LIGHT0 + id, GL_SPOT_CUTOFF, angle);
    }*/
}
void GL4Context::setLightSpotExponent(unsigned int id, float exponent)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        m_lights[id].exponent = exponent*128.0f;
        //glLightf(GL_LIGHT0 + id, GL_SPOT_EXPONENT, m_lights[id].exponent);
    }*/
}
void GL4Context::getLightPosition(unsigned int id, Vector4 * position)
{
    /*if(id < MAX_MRCLIGHTS)
        (*position) = m_lights[id].position;*/
}
void GL4Context::getLightDiffuse(unsigned int id, Vector4 * diffuse)
{
    /*if(id < MAX_MRCLIGHTS)
        (*diffuse) = m_lights[id].diffuse;*/
}
void GL4Context::getLightSpecular(unsigned int id, Vector4 * specular)
{
    /*if(id < MAX_MRCLIGHTS)
        (*specular) = m_lights[id].specular;*/
}
void GL4Context::getLightAmbient(unsigned int id, Vector4 * ambient)
{
    /*if(id < MAX_MRCLIGHTS)
        (*ambient) = m_lights[id].ambient;*/
}
void GL4Context::getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic)
{
    /*if(id < MAX_MRCLIGHTS)
    {
        (*constant) = m_lights[id].constant;
        (*linear) = m_lights[id].linear;
        (*quadratic) = m_lights[id].quadratic;
    }*/
}
void GL4Context::getLightSpotDirection(unsigned int id, Vector3 * direction)
{
    /*if(id < MAX_MRCLIGHTS)
        (*direction) = m_lights[id].direction;*/
}
void GL4Context::getLightSpotAngle(unsigned int id, float * angle)
{
    /*if(id < MAX_MRCLIGHTS)
        (*angle) = m_lights[id].angle;*/
}
void GL4Context::getLightSpotExponent(unsigned int id, float * exponent)
{
    /*if(id < MAX_MRCLIGHTS)
        (*exponent) = m_lights[id].exponent;*/
}

// blending
void GL4Context::enableBlending(void)	{ glEnable (GL_BLEND); }
void GL4Context::disableBlending(void)	{ glDisable(GL_BLEND); }
void GL4Context::setBlendingMode(BLENDING_MODES mode)
{
    switch(mode)
    {
		case BLENDING_NONE:
            glBlendFunc(GL_ONE, GL_ZERO);
            break;

		case BLENDING_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

		case BLENDING_ADD:
            glBlendFunc(GL_ONE, GL_ONE);
            break;

		case BLENDING_PRODUCT:
            glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            break;

		case BLENDING_LIGHT:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            break;

        default:
                        break;
    }
}

// point size
void GL4Context::setPointSize(float size)
{

}

// VBO
void GL4Context::createVBO(unsigned int * vboId)
{
    glGenBuffers(1, vboId);
}

void GL4Context::deleteVBO(unsigned int * vboId)
{
    glDeleteBuffers(1, vboId);
}

void GL4Context::bindVBO(VBO_TYPES type, unsigned int vboId)
{
	GLenum gltype = type == VBO_ARRAY ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
    glBindBuffer(gltype, vboId);
}

void GL4Context::setVBO(VBO_TYPES type, const void * data, unsigned int size, VBO_MODES mode)
{
	GLenum gltype = type == VBO_ARRAY ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
    switch(mode)
    {
		case VBO_STATIC:
            glBufferData(gltype, size, data, GL_STATIC_DRAW);
            break;
		case VBO_DYNAMIC:
            glBufferData(gltype, size, data, GL_DYNAMIC_DRAW);
            break;
		case VBO_STREAM:
            glBufferData(gltype, size, data, GL_STREAM_DRAW);
            break;
    }
}

void GL4Context::setVBOSubData(VBO_TYPES type, unsigned int offset, const void * data, unsigned int size)
{
	GLenum gltype = type == VBO_ARRAY ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
    glBufferSubData(gltype, offset, size, data);
}

void GL4Context::createVAO(unsigned int* vaoId)
{
    glGenVertexArrays(1, vaoId);
}

void GL4Context::deleteVAO(unsigned int* vaoId)
{

}

void GL4Context::bindVAO(unsigned int vaoId)
{
    glBindVertexArray(vaoId);
}

void GL4Context::selectSubroutine(unsigned int fx, unsigned int type, const char* routine)
{
	if (type == M_SHADER_PIXEL)
		type = GL_FRAGMENT_SHADER;
	else
		type = GL_VERTEX_SHADER;

	GLuint index = glGetSubroutineIndex(fx, type, routine);
	glUniformSubroutinesuiv(type, 1, &index);
}

void GL4Context::enablePolygonOffset(float x, float y)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(x, y);
}

void GL4Context::disablePolygonOffset()
{
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
}


void GL4Context::setPolygonMode(PRIMITIVE_TYPES t)
{
	switch(t)
	{
		case PRIMITIVE_LINES: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
		
		case PRIMITIVE_POINTS: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
		
		default:
		case PRIMITIVE_TRIANGLES: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}
#endif

int GL4Context::getUniformLocation(unsigned int fx, const char* name)
{
	// return glGetUniformLocation(fx, name);
	auto shader = m_uniformLocationCache.find(fx);
	if(shader == m_uniformLocationCache.end())
	{
		new_element:
		GLint id = glGetUniformLocation(fx, name);
		m_uniformLocationCache[fx][name] = id;
		return id;
	}

	auto uniform = shader->second.find(name);
	if(uniform == shader->second.end())
		goto new_element;

	return uniform->second;
}

/* * Copyright (C) 2014
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef __MPOST_PROCESSOR_H__
#define __MPOST_PROCESSOR_H__

#include "MVariable.h"
#include <stdint.h>

class M_ENGINE_EXPORT MPostProcessor
{
public:
    MPostProcessor();

    bool draw(MOCamera* camera);

    bool loadShader(const char* vertShad, const char* fragShad);
    bool loadShaderFile(const char* vertShad, const char* fragShad);

    void updateResolution();
    void eraseTextures();

    void addFloatUniform(const char* name);
    void setFloatUniformValue(const char* name, float value);
    float getFloatUniformValue(int idx);

    void addIntUniform(const char* name);
    void setIntUniformValue(const char* name, float value);
    int getIntUniformValue(int idx);

    int getNumUniforms() { return m_UniformList.size(); }
    const char* getUniformName(int idx);
    M_VARIABLE_TYPE getUniformType(int idx);

    void clear();

    const char* getVertexShader() { return m_vertShadPath.getSafeString(); }
    const char* getFragmentShader() { return m_fragShadPath.getSafeString(); }

    void setShaderPath(const char* vertPath, const char* fragPath);
    int getFX() { return m_fx; }

    float getResolutionMultiplier() { return m_ResolutionMultiplier; }
    void setResolutionMultiplier(float res) { m_ResolutionMultiplier = res; }

protected:
    void set2D(unsigned int w, unsigned int h);
    void drawQuad(MVector2 scale);

    void sendUniforms();

    unsigned int m_BufferID;
    unsigned int m_ColourTexID;
    unsigned int m_DepthTexID;

    unsigned int m_fx;
    unsigned int m_fragShad;
    unsigned int m_vertShad;

    int m_Resolution;
    float m_ResolutionMultiplier;

    MString m_vertShadPath;
    MString m_fragShadPath;

    typedef struct
    {
        MVariable variable;
        char name[255];
        bool dirty;
    }uniform_t;

    typedef struct
    {
        MVariable variable;
        char name[255];
        bool dirty;
        float value;
    }float_uniform_t;

    typedef struct
    {
        MVariable variable;
        char name[255];
        bool dirty;
        int value;
    }int_uniform_t;

    vector<uintptr_t> m_UniformList;
};

#endif

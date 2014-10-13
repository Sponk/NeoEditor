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

/**
 * @brief The MPostProcessor class contains functionality used to render
 * the current scene to a texture to allow applying GLSL post effects when
 * rendering the final image to the screen.
 */
class M_ENGINE_EXPORT MPostProcessor
{
public:
    MPostProcessor();

    /**
     * @brief Renders the current scene to a texture and then to the screen
     * after applying the post effects shader.
     * @param camera The camera to use.
     * @return Returns <strong>true</strong> if it rendered properly, false if it didn't.
     */
    bool draw(MOCamera* camera);

    /**
     * @brief Loads, compiles and links a shader pair to the GPU.
     * @param vertShad The sources of the vertex shader.
     * @param fragShad The sources of the fragment shader.
     * @return Returns <strong>true</strong> if both shader loaded properly, false if they didn't.
     */
    bool loadShader(const char* vertShad, const char* fragShad);

    /**
     * @brief Loads, compiles and links a shader pair to the GPU.
     * @param vertShad The path to the file containing the vertex shader sources.
     * @param fragShad The path to the file containing the fragment shader sources.
     * @return Returns <strong>true</strong> if both shader loaded properly, false if they didn't.
     */
    bool loadShaderFile(const char* vertShad, const char* fragShad);

    /**
     * @brief Updates the color and depth texture to fit the current screen/window resolution.
     * Needs to be called <strong>after</strong> MPostProcessor::eraseTextures()!
     */
    void updateResolution();

    /**
     * @brief Deletes the color and depth texture. That allows them to be recreated for example when
     * the resolution changes.
     */
    void eraseTextures();

    /**
     * @brief Adds a uniform of the type float to the uniform list.
     * @param name The name of the new uniform variable.
     */
    void addFloatUniform(const char* name);

    /**
     * @brief Sets the value of a uniform with the type float.
     * @param name The name of the uniform. This is the same name given when calling MPostProcessor::addFloatUniform().
     * @param value The value to apply.
     */
    void setFloatUniformValue(const char* name, float value);

    /**
     * @brief Gets the float value of the uniform at the index.
     * @param idx The index of the uniform.
     * @return The float value
     */
    float getFloatUniformValue(int idx);

    /**
     * @brief Adds a uniform of the type int to the uniform list.
     * @param name The name of the new uniform variable.
     */
    void addIntUniform(const char* name);

    /**
     * @brief Sets the value of a uniform with the type int.
     * @param name The name of the uniform. This is the same name given when calling MPostProcessor::addIntUniform().
     * @param value The value to apply.
     */
    void setIntUniformValue(const char* name, float value);

    /**
     * @brief Gets the int value of the uniform at the index.
     * @param idx The index of the uniform.
     * @return The int value
     */
    int getIntUniformValue(int idx);

    /**
     * @brief Returns the number of currently registered uniform variables.
     * Increases whenever one of the "add*Uniform" gets called.
     * @return The number of all registered uniforms.
     */
    int getNumUniforms() { return m_UniformList.size(); }

    /**
     * @brief Returns the name of the specified uniform variable.
     * @param idx The index of the uniform variable.
     * @return The name of the uniform variable.
     */
    const char* getUniformName(int idx);

    /**
     * @brief Returns the type of the specified uniform variable.<br>
     * See also: M_VARIABLE_TYPE
     * @param idx The index of the uniform variable.
     * @return The type of the uniform variable.
     */
    M_VARIABLE_TYPE getUniformType(int idx);

    /**
     * @brief Deletes all uniform variables currently registered.
     */
    void clear();

    /**
     * @return The vertex shaders file path
     */
    const char* getVertexShader() { return m_vertShadPath.getSafeString(); }

    /**
     * @return The fragment shaders file path
     */
    const char* getFragmentShader() { return m_fragShadPath.getSafeString(); }

    /**
     * @brief Sets the path to the vertex shader and the fragment shader without
     * loading them.
     *
     * @param vertPath The path to the vertex shader.
     * @param fragPath The path to the fragment shader.
     */
    void setShaderPath(const char* vertPath, const char* fragPath);

    /**
     * @return The internal FX ID.
     */
    int getFX() { return m_fx; }

    /**
     * @return The resolution multiplier.
     */
    float getResolutionMultiplier() { return m_ResolutionMultiplier; }

    /**
     * @brief Sets the current resolution multiplier.
     *
     * For this value to apply you need to call MPostProcessor::eraseTextures() and MPostProcessor::updateResolution().<br>
     * This value is multiplied with the screen resolution, that means that value = 1.0 is the full resolution
     * and value = 0.5 is the half of the full resolution etc.
     * @param res The new resolution multiplier.
     */
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

    // Cache texture coordinates for faster rendering
    MVector2 m_texCoords[4];

    // Cache vertices for faster rendering
    MVector2 m_vertices[4];
};

#endif

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2016 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef __MATERIAL_H
#define __MATERIAL_H

namespace Neo
{
/**
 * @brief The texture pass class implements some basic primitives to
 * manage and use multi-texture materials.
 *
 * Examples for texture passes are the diffuse texture, normal map and
 * specular map.
 */
class NEO_ENGINE_EXPORT TexturePass
{
public:
	TexturePass(Texture *texture, TEX_COMBINE_MODES combineMode,
				unsigned int mapChannel)
		: m_mapChannel(mapChannel),
		  m_texture(texture),
		  m_combineMode(combineMode)
	{
	}

	TexturePass(const TexturePass &texturePass)
		: m_mapChannel(texturePass.m_mapChannel),
		  m_texture(texturePass.m_texture),
		  m_combineMode(texturePass.m_combineMode)
	{
	}

private:
	// texture
	unsigned int m_mapChannel;
	Texture *m_texture;

	// combine mode
	TEX_COMBINE_MODES m_combineMode;

public:
	/**
	 * @brief Sets the map channel.
	 * @param mapChannel The new map channel.
	 */
	inline void setMapChannel(unsigned int mapChannel)
	{
		m_mapChannel = mapChannel;
	}

	/**
	 * @brief Sets the texture to use.
	 * @param texture The texture.
	 */
	inline void setTexture(Texture *texture) { m_texture = texture; }

	/**
	 * @brief Returns the map channel.
	 * @return The map channel.
	 */
	inline unsigned int getMapChannel(void) { return m_mapChannel; }

	/**
	 * @brief Returns the texture that is used.
	 * @return The texture.
	 */
	inline Texture *getTexture(void) { return m_texture; }

	/**
	 * @brief Sets the combine mode when using multiple passes
	 * for one material.
	 * 
	 * @param combineMode The combine mode.
	 * @see TEX_COMBINE_MODES
	 */
	inline void setCombineMode(TEX_COMBINE_MODES combineMode)
	{
		m_combineMode = combineMode;
	}

	/**
	 * @brief Returns the combine mode.
	 * @return The combine modes.
	 * @see TEX_COMBINE_MODES
	 */
	inline TEX_COMBINE_MODES getCombineMode(void) { return m_combineMode; }
};

/**
 * @brief The Material class contains the implementation of a material used
 * by 3D meshes for rendering different looking surfaces.
 *
 * @see Mesh
 */
class NEO_ENGINE_EXPORT Material
{
public:
	Material(void);
	Material(const Material &material);
	~Material(void);

private:
	// type
	int m_type;

	// FX
	FXRef *m_FXRef;
	FXRef *m_ZFXRef;

	// blend mode
	BLENDING_MODES m_blendMode;

	// properties
	float m_opacity;
	float m_shininess;
	float m_roughness;
	float m_customValue;
	Vector3 m_diffuse;
	Vector3 m_specular;
	Vector3 m_emit;
	Vector3 m_customColor;

	// textures pass
	unsigned int m_texturesPassNumber;
	TexturePass **m_texturesPass;

public:

	/**
	 * @brief Resets the texture pass counter.
	 *
	 * @warning This can produce memory leaks when used in the wrong context!
	 */
	void resetTexturesPassNumber() { m_texturesPassNumber = 0; }

	/**
	 * @brief Sets the type of the material.
	 * @param type The type value.
	 */
	inline void setType(int type) { m_type = type; }

	/**
	 * @brief Returns the type of the material.
	 * @return The material type.
	 */
	inline int getType(void) { return m_type; }

	// FX
	inline void setFXRef(FXRef *FXRef) { m_FXRef = FXRef; }
	inline void setZFXRef(FXRef *ZFXRef) { m_ZFXRef = ZFXRef; }
	inline FXRef *getFXRef(void) { return m_FXRef; }
	inline FXRef *getZFXRef(void) { return m_ZFXRef; }

	/**
	 * @brief Changes the current blend mode.
	 * @param blendMode the blend mode.
	 *
	 * @see BLENDING_MODES
	 */
	inline void setBlendMode(BLENDING_MODES blendMode)
	{
		m_blendMode = blendMode;
	}

	/**
	 * @brief Returns the current blend mode.
	 * @return the Blend mode.
	 *
	 * @see BLENDING_MODES
	 */
	inline BLENDING_MODES getBlendMode(void) { return m_blendMode; }

	/**
	 * @brief Sets the opacity value.
	 *
	 * An opacity value of 1.0f means fully opaque, 0.0 means fully
	 * transparent.
	 *
	 * @param opacity The opacity value.
	 */
	inline void setOpacity(float opacity) { m_opacity = opacity; }

	/**
	 * @brief Sets the shininess value.
	 *
	 * A higher shininess value gives smaller and brighter highlights.
	 *	 
	 * @param shininess The shininess value.
	 */
	inline void setShininess(float shininess)
	{
		m_shininess = shininess;
		m_roughness = (1 - shininess/1024.0f); //sqrt(2.0f/(shininess + 2));
		m_roughness *= m_roughness;
	}

	inline void setRoughness(float roughness)
	{
		m_shininess = 2.0f/(roughness*roughness) - 2;
		m_roughness = roughness;
	}

	float getRoughness() const { return m_roughness; }

	/**
	 * @brief Sets the custom value.
	 * @param customValue The new value.
	 */
	inline void setCustomValue(float customValue)
	{
		m_customValue = customValue;
	}

	/**
	 * @brief Sets the diffuse color.
	 *
	 * The color is given as RGB between 0.0f and 1.0f.
	 * @param diffuse The color.
	 */
	inline void setDiffuse(const Vector3 &diffuse) { m_diffuse = diffuse; }

	/**
	 * @brief Sets the specular color.
	 *
	 * The color is given as RGB between 0.0f and 1.0f.
	 * @param specular The color.
	 */
	inline void setSpecular(const Vector3 &specular) { m_specular = specular; }

	/**
	 * @brief Sets the emit color.
	 *
	 * The color is given as RGB between 0.0f and 1.0f.
	 * @param emit The color.
	 */
	inline void setEmit(const Vector3& emitval) { m_emit = emitval; }

	/**
	 * @brief Sets the custom color.
	 *
	 * The color is given as RGB between 0.0f and 1.0f.
	 * @param customColor The color.
	 */
	inline void setCustomColor(const Vector3 &customColor)
	{
		m_customColor = customColor;
	}

	/**
	 * @brief Returns the current opacity.
	 * @return The current opacity.
	 */
	inline float getOpacity(void) { return m_opacity; }

	/**
	 * @brief Returns the current shininess.
	 * @return The current shininess.
	 */
	inline float getShininess(void) { return m_shininess; }

	/**
	 * @brief Returns the current custom value.
	 * @return The current custom value.
	 */
	inline float getCustomValue(void) { return m_customValue; }

	/**
	 * @brief Returns the current emit color.
	 * @return The current emit color.
	 */
	inline Vector3 getEmit(void) const { return m_emit; }

	/**
	 * @brief Returns the current diffuse color.
	 * @return The current diffuse color.
	 */
	inline Vector3 getDiffuse(void) const { return m_diffuse; }

	/**
	 * @brief Returns the current specular color.
	 * @return The current specular color.
	 */
	inline Vector3 getSpecular(void) const { return m_specular; }

	/**
	 * @brief Returns the current custom color.
	 * @return The current custom color.
	 */
	inline Vector3 getCustomColor(void) const { return m_customColor; }

	/**
	 * @brief Allocates enough memory for all texture passes.
	 * @param size The number of texture passes to allocate.
	 */
	void allocTexturesPass(unsigned int size);

	/**
	 * @brief Deletes all texture passes.
	 */
	void clearTexturesPass(void);

	/**
	 * @brief Adds a texture pass to the internal array.
	 * @param texture The texture to append.
	 * @param combineMode The combine mode for multi-texturing.
	 * @param mapChannel The map channel to append the texture to.
	 */
	void addTexturePass(Texture *texture, TEX_COMBINE_MODES combineMode,
						unsigned int mapChannel);

	/**
	 * @brief Returns the number of texture passes.
	 * @return The number of texture passes.
	 */
	inline unsigned int getTexturesPassNumber(void)
	{
		return m_texturesPassNumber;
	}

	/**
	 * @brief Returns a texture pass by ID from the internal array.
	 * @param id The index into the array.
	 */
	inline TexturePass *getTexturePass(unsigned int id)
	{
		return m_texturesPass[id];
	}
};
}
#endif

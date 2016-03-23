//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __OLIGHT_H
#define __OLIGHT_H

namespace Neo
{

// light types
enum LIGHT_TYPES
{
	LIGHT_POINT = 0,
	LIGHT_SPOT,
	LIGHT_DIRECTIONAL
};

/**
 * @brief Represents a 3D light in the scene.
 *
 * How to create a new OLight:
 * @code
 * Level* level = NeoEngine::getInstance()->getLevel();
 * Scene* scene = level->getCurrentScene();
 * OLight* newEntity = scene->addNewLight();
 * @endcode
 * @see Object3d
 */
class NEO_ENGINE_EXPORT OLight : public Object3d
{
public:

	// constructor / destructor
	OLight(void);
	~OLight(void);

	// copy constructor
	OLight(const OLight & light);

private:

	// lightType
	LIGHT_TYPES m_lightType;
	
	// radius
	float m_radius;

	// color
	float m_intensity;
	Vector3 m_color;
	
	// spot
	float m_spotAngle;
	float m_spotExponent;
	
	// shadow
	bool m_shadow;
	float m_shadowBias;
	float m_shadowBlur;
	unsigned int m_shadowQuality;

public:

	// type
	virtual int getType(void){ return OBJECT3D_LIGHT; }

	/**
	 * @brief Changes the light type the OLight has.
	 * @param lightType The new light type.
	 */
	inline void setLightType(LIGHT_TYPES lightType) { m_lightType = lightType; }
	
	/**
	 * @brief Returns the light type.
	 * @return The light type.
	 */
	inline LIGHT_TYPES getLightType(void) { return m_lightType; }

	/**
	 * @brief Change if the light should cast shadows.
	 * @param shadow The new value.
	 */
	inline void castShadow(bool shadow) { m_shadow = shadow; }

	/**
	 * @brief Changes the shadow quality.
	 *
	 * Powers of two like 1024 or 4096 are to be preferred.
	 * @param shadowQuality The new shadow quality.
	 */
	inline void setShadowQuality(unsigned int shadowQuality)
	{
		m_shadowQuality = shadowQuality;
	}

	/**
	 * @brief Changes the shadow bias.
	 *
	 * The shadow bias controls the offset that the shadow uses to prevent
	 * self shadowing of an object.
	 *
	 * @param shadowBias The new shadow bias value.
	 */
	inline void setShadowBias(float shadowBias) { m_shadowBias = shadowBias; }

	/**
	 * @brief Changes the shadow blur factor..
	 * @param shadowBlur The new shadow blur value.
	 */	
	inline void setShadowBlur(float shadowBlur) { m_shadowBlur = shadowBlur; }

	/**
	 * @brief Checks if the OLight is casting shadows.
	 * @return A boolean value.
	 */
	inline bool isCastingShadow(void) { return m_shadow; }

	/**
	 * @brief Returns the shadow quality of the OLight.
	 * @return The shadow quality.
	 * @see setShadowQuality
	 */
	inline unsigned int getShadowQuality(void) { return m_shadowQuality; }
	
	/**
	 * @brief Returns the shadow bias.
	 * @return The shadow bias.
	 * @see setShadowBias
	 */
	inline float getShadowBias(void) { return m_shadowBias; }
	
	/**
	 * @brief Returns the shadow blur.
	 * @return The shadow blur.
	 * @see setShadowBlur
	 */
	inline float getShadowBlur(void) { return m_shadowBlur; }

	/**
	 * @brief Changes the light radius in which the OLight object takes effect.
	 * @param radius The radius.
	 */
	inline void setRadius(float radius) { m_radius = radius; }
	
	/**
	 * @brief Returns the light radius.
	 * @return The radius.
	 */
	inline float getRadius(void) { return m_radius; }

	/**
	 * @brief Changes the light intensity.
	 * @param intensity The intensity.
	 */
	inline void setIntensity(float intensity) { m_intensity = intensity; }
	
	/**
	 * @brief Changes the light color.
	 * @param color The new light color.
	 */
	inline void setColor(const Vector3 &color) { m_color = color; }
	
	/**
	 * @brief Returns the light intensity.
	 * @return The intensity.
	 */
	inline float getIntensity(void) { return m_intensity; }
	
	/**
	 * @brief Returns the light color.
	 * @return The light color.
	 */
	inline Vector3 getColor(void) const { return m_color; }
	
	/**
	 * @brief Returns the light color how it appears with the chosen intensity.
	 * @return lightColor * intensity as a Vector3.
	 */ 
	inline Vector3 getFinalColor(void) const { return m_color * m_intensity; }

	/**
	 * @brief Changes the spot angle.
	 *
	 * Values are clamped between 0.0f and 180.0f.
	 * @param angle The angle in degrees.
	 */
	inline void setSpotAngle(float angle)
	{
		m_spotAngle = CLAMP(angle, 0.0f, 180.0f);
	}

	/**
	 * @brief Changes the exponent value.
	 * @param exponent The new exponent value.
	 */
	inline void setSpotExponent(float exponent)
	{
		m_spotExponent = CLAMP(exponent, 0.0f, 1.0f);
	}

	/**
	 * @brief Returns the spot angle.
	 * @return The spot angle in degrees.
	 */
	inline float getSpotAngle(void) { return m_spotAngle; }
	
	/**
	 * @brief Returns the spot exponent.
	 * @return The spot exponent.
	 */
	inline float getSpotExponent(void) { return m_spotExponent; }

	// visibility
	virtual void updateVisibility(OCamera *camera);
};
}
#endif

/**
 * @brief Returns the color of the given light source.
 *
 * The color is encoded as a vec3.
 * @param object The light source.
 * @return The color.
 */
getLightColor(object);

/**
 * @brief Returns the radius of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightRadius(object);

/**
 * @brief Returns the intensity of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightIntensity(object);

/**
 * @brief Returns the shadow quality of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightShadowQuality(object);

/**
 * @brief Returns the bias of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightShadowBias(object);

/**
 * @brief Returns the blur factor of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightShadowBlur(object);

/**
 * @brief Returns the spot angle of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightSpotAngle(object);

/**
 * @brief Returns the spot exponent of the given light source.
 * @param object The light source.
 * @return A number.
 */
getLightSpotExponent(object);

/**
 * @brief Changes the color of the given light source.
 * @param object The light source.
 * @param color A vec3 containing the RGB values.
 *
 * @startcode
 * setLightColor(light, {0.5,0.5,0.5})
 * @endcode
 */
setLightColor(object, color);

/**
 * @brief Changes the radius of the given light source.
 * @param object The light source.
 * @param radius A number.
 */
setLightRadius(object, radius);

/**
 * @brief Changes the intensity of the given light source.
 * @param object The light source.
 * @param intensity A number.
 */
setLightIntensity(object, intensity);

/**
 * @brief Changes the shadow quality of the given light source.
 *
 * The shadow quality is basically the resolution of the texture
 * the depth data will be rendered to. The resolution looks like "quality x quality"
 * and works best if quality is a power of two like 1024 or 2048.
 *
 * @param object The light source.
 * @param quality A number. Works best with a power of two.
 */
setLightShadowQuality(object, quality);

/**
 * @brief Changes the shadow bias of the given light source.
 * @param object The light source.
 * @param bias A number.
 */
setLightShadowBias(object, bias);

/**
 * @brief Changes the shadow blur of the given light source.
 *
 * The shadow blur describes the number of passes the GLSL shader
 * uses to blur the shadow edge. Bigger values might slow down your game!
 *
 * @param object The light source.
 * @param blur A number. Keep this as small as possible!
 */
setLightShadowBlur(object, blur);

/**
 * @brief Changes the spot angle of the given light source.
 *
 * The spot angle describes the angle a spot light is able to illuminate.
 * A point light has an angle > 180° and can be turned into a spot light
 * by setting a smaller spot angle.
 *
 * @param object The light source.
 * @param spotAngle A number.
 */
setLightSpotAngle(object, spotAngle);

/**
 * @brief Changes the spot exponent of the given light source.
 *
 * The spot exponent describes the strength of blending out the
 * illuminated area of the spot light on the edges. 
 *
 * @param object The light source.
 * @param exponent A number.
 */
setLightSpotExponent(object, exponent);

/**
 * @brief Changes the string displayed by a text object.
 * @param object The text object.
 * @param text The new string.
 *
 * Example:
 * @code setText(textobject, "Sometext!")@endcode
 */
setText(object , text);

/**
 * @brief Retrieves the string displayed by a text object.
 * @param object The text object.
 * @return The current string.
 */
getText(object);

/**
 * @brief Retrieves the color displayed by a text object.
 * @param object The text object.
 * @return The current color as a vec4.
 *
 * Example:
 * @code
 * color = getTextColor(object)
 * print("Color: R = " .. color[1] .. " G = " .. color[2] .. " B = " .. color[3] .. " A = " .. color[4])
 * @endcode
 */
getTextColor(object);

/**
 * @brief Sets the color displayed by a text object.
 * @param object The text object.
 * @param color The new color as a vec4.
 *
 * Example:
 * @code
 * setTextColor(object, {r,g,b,a})
 * @endcode
 */
setTextColor(object, color);

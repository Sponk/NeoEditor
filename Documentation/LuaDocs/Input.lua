/**
 * @brief Checks, if the given key is currently pressed.
 *
 * See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
 * @param The name of the key.
 * @return A boolean value.
 */
isKeyPressed(key);

/**
 * @brief Checks, if the given key got pressed in this frame.
 *
 * See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
 * @param The name of the key.
 * @return A boolean value.
 */
onKeyDown(key);

/**
 * @brief Checks, if the given key got released in this frame.
 *
 * See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
 * @param The name of the key.
 * @return A boolean value.
 */
onKeyUp(key);

/**
 * @brief Retrieves the current axis value of a joystick or mouse.
 *
 * The value is between 0.0 and 1.0 <br>
 *
 * See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
 * @param The name of the key.
 * @return A float value.
 */
getAxis(axis);
getProperty(name);

/**
 * @brief Retrieves the current touch position on touch devices.
 *
 * Is the same as the mouse cursor position since SDL makes no difference between them.
 * @param The touch device ID
 * @return The touch position.
 */
getTouchPosition(touchId);
getLastTouchPosition(touchId);
getTouchPhase(touchId); 

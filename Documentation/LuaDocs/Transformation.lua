/**
 * @brief Rotates an object.
 *
 * The local parameter is optional.<br>
 * Example:<br><br>
 *
 * -- Rotate around X-Axis<br>
 * rotate(object, {1,0,0}, 90)<br>
 *
 * @param object The object to rotate.
 * @param axis An vec3 containing the axis to rotate around.
 * @param angle The angle to rotate.
 * @param local Specifies if the rotation is in the global or the local space.
 */
rotate(object, axis, angle, "local");

/**
 * @brief Translates an object.
 *
 * The local parameter is optional.<br>
 * Example:<br><br>
 *
 * @code
 * -- Translate on X-Axis in the local space<br>
 * rotate(object, {15,0,0}, "local")<br>
 * @endcode
 *
 * @param object The object to translate.
 * @param axis An vec3 containing the direction to translate to.
 * @param local Specifies if the translation is in the global or the local space.
 */
translate(object, {x, y, z}, "local");

/**
 * @brief Retrieves the position of an object.
 * @param object The object.
 * @return A vec3 containing the current position.
 */
getPosition(object);

/**
 * @brief Retrieves the rotation of an object.
 * @param object The object.
 * @return A vec3 containing the current rotation.
 */
getRotation(object);

/**
 * @brief Retrieves the scale of an object.
 * @param object The object.
 * @return A vec3 containing the current scale.
 */
getScale(object);

/**
 * @brief Sets the position of an object.
 * @param object The object.
 * @param pos A vec3 containing the new position.
 */
setPosition(object, pos);

/**
 * @brief Sets the rotation of an object.
 * @param object The object.
 * @param rot A vec3 containing the new rotation.
 */
setRotation(object, rot);

/**
 * @brief Sets the scale of an object.
 * @param object The object.
 * @param scale A vec3 containing the new scale.
 */
setScale(object, scale);

/**
 * @brief Retrieves the transformed position of an object.
 *
 * This includes all transformations done by parents.
 *
 * @param object The object.
 * @return A vec3 containing the current position.
 */
getTransformedPosition(object);

/**
 * @brief Retrieves the transformed rotation of an object.
 *
 * This includes all transformations done by parents.
 *
 * @param object The object.
 * @return A vec3 containing the current rotation.
 */
getTransformedRotation(object);

/**
 * @brief Retrieves the transformed scale of an object.
 *
 * This includes all transformations done by parents.
 *
 * @param object The object.
 * @return A vec3 containing the current scale.
 */
getTransformedScale(object);
getInverseRotatedVector(object);
getRotatedVector(object);
getInverseVector(object);
getTransformedVector(object);

/**
 * @brief Recalculates the matrix of the given object.
 * @param object The object.
 */
updateMatrix(object);

/**
 * @brief Retrieves the matrix of an object as a table.
 *
 * @param object The object.
 * @return A table containing the current matrix.
 */
getMatrix(object);

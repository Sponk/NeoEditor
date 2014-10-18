/**
 * @brief Gets the object with the given name.
 *
 * Can also give objects from other scenes than the current one.
 * @param scene The scene to look in. This parameter is optional.
 * @param name The name of the object to retrieve.
 */
getObject(scene, name);

/**
 * @brief Retrieves the parent of an object.
 * @param The object to retrieve the parent from. See also: getObject
 * @return The parent object.
 */
getParent(object);

/**
 * @brief Returns a list of all childrens from the given object.
 * @param object The parent object.
 * @return The list of all children.
 */
getChilds(object);

/**
 * @brief Creates a clone of the given object.
 * @param object The object to clone.
 * @return The clone.
 */
getClone(object);

/**
 * @brief Changes the parent of the given object.
 *
 * Resets parent-child relationship if parent = nil.
 * @param object The object.
 * @param parent The new parent.
 */
setParent(object, parent);

/**
 * @brief Retrieves the name of the given object.
 * @param object The object.
 * @return The name of the object.
 */
getName(object);

/**
 * @brief Activates the given object.
 * @param object The object.
 */
activate(object);

/**
 * @brief Deactivates the given object.
 *
 * This function does <strong>not</strong> delete the object from memory!
 * It will just set invisible and removed from the physics simulation.
 * @param object The object.
 */
deactivate(object);

/**
 * @brief Checks if the given object is visible.
 * @param object The object.
 * @return A boolean value.
 */
isVisible(object);

/**
 * @brief Checks if the given object is active.
 * @param object The object.
 * @return A boolean value.
 */
isActive(object);

/**
 * @brief Specifies, if a light renders shadows or if an entity has a shadow.
 * @param object The entity/light.
 * @param shadow A boolean value.
 */
enableShadow(object, shadow);

/**
 * @brief Checks if a light renders shadows or if an entity has a shadow.
 * @param object The entity/light.
 * @return A boolean value.
 */
isCastingShadow(object); 

/**
 * @brief Sets the current gravity.
 *
 * Example:<br>
 * @code
 *  setGravity({0,0,-0.5})
 * @endcode
 * @param vec3 A vec3 containing the new gravitational pull.
 */
setGravity(vec3);

/**
 * @brief Returns the gravity in the current scene.
 * @return A vec3 containing the gravitational pull.
 */
getGravity();

/**
 * @brief Returns the linear damping from the given object.
 * @param object The physically active object
 * @return A number containing the linear damping.
 */
getLinearDamping(object);

/**
* @brief Sets the linear damping of the given object.
* @param object The physically active object
* @param damping A number containing the linear damping.
*/
setLinearDamping(object, damping);

/**
 * @brief Returns the angular damping from the given object.
 * @param object The physically active object 
 * @return A number containing the angular damping.
 */
getAngularDamping(object);

/**
 * @brief Sets the angular damping of the given object.
 * @param object The physically active object
 * @param damping A number containing the angular damping.
 */
setAngularDamping(object, damping);

/**
 * @brief Returns the linear factor from the given object.
 * @param object The physically active object
 * @return A vec3 containing the linear factor.
 */
getLinearFactor(object);

/**
* @brief Sets the linear factor of the given object.
* @param object The physically active object
* @param factor A vec3 containing the linear factor.
*/
setLinearFactor(object, factor);

/**
 * @brief Returns the angular factor from the given object.
 * @return A number containing the linear factor.
 */
getAngularFactor(object);

/**
* @brief Sets the angular factor of the given object.
* @param object The physically active object
* @param factor A number containing the angular factor.
*/
setAngularFactor(object, factor);

/**
 * @brief Returns the mass of the given object.
 * @param object The physically active object.
 * @return A number.
 */
getMass(object);

/**
 * @brief Sets the mass of the given object.
 * @param object The physically active object.
 * @param mass The new mass.
 */
setMass(object, mass);

/**
 * @brief Returns the friction constant of the given object.
 * @param object The physically active object.
 * @return A number.
 */
getFriction(object);

/**
 * @brief Sets the friction constant of the given object.
 * @param object The physically active object.
 * @param friction The new friction.
 */
setFriction(object, friction);

/**
 * @brief Returns the physical restitution of the given object.
 * @param object The physically active object.
 * @return A number.
 */
getRestitution(object);

/**
 * @brief Sets the restitution of the given object.
 * @param object The physically active object.
 * @param restitution The new restitution.
 */
setRestitution(object, restitution);

/**
 * @brief Removes all active forces from the given object.
 * @param object The physically activated object.
 */
clearForces(object);

/**
 * @brief Applies a new central force to the given object.
 *
 * The new force will be added to the currently existing central force of the
 * object. The mode parameter is optional and allows you to apply forces relative to the objects
 * current rotation.
 *
 * @param object The physically active object.
 * @param force A vec3 containing the force to apply.
 * @param mode A string containing "local". Can be left out.
 */
addCentralForce(object, force, mode);

/**
 * @brief Applies a new torque to the given object.
 *
 * The new torque will be added to the currently existing torque of the
 * object. The mode parameter is optional and allows you to apply a torque relative to the objects
 * current rotation.
 *
 * @param object The physically active object.
 * @param torque A vec3 containing the torque to apply.
 * @param mode A string containing "local". Can be left out.
 */
addTorque(object, torque, mode);

/**
 * @brief Returns the force applied to the given object.
 * @param object The physically active object.
 * @return A vec3.
 */
getCentralForce(object);

/**
 * @brief Returns the torque applied to the given object.
 * @param object The physically active object.
 * @return A vec3.
 */
getTorque(object);

/**
 * @brief Checks if the object is colliding at all.
 * @param object The physically active object.
 * @return A boolean.
 */
isCollisionTest(object);

/**
 * @brief Checks if the two given objects are colliding.
 * @param object1 The physically active object.
 * @param object2 The physically active object.
 * @return A boolean.
 */
isCollisionBetween(object1, object2);

/**
 * @brief Returns the total number of collisions the given object currently has.
 * @param object The physically active object.
 * @return A number.
 */
getNumCollisions(object);
rayHit(start, end);
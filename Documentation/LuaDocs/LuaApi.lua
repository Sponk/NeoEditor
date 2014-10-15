
/**
 * @brief Creates a 3D vector.
 * @param x The X value
 * @param y The Y value
 * @param z The Z value
 * @return The newly created 3D vector
 */
vec3(float x, float y, float z);

/**
 * @brief Calculates the length of a 3D vector.
 * @param The 3D vector to use.
 * @return The length of the vector.
 */
length(vec3);

/**
 * @brief Normalizes a vector
 *
 * This function takes the input vector and transforms it
 * to a new vector with the length 1.0f.
 *
 * @param The vector to normalize.
 * @return The normalized vec3.
 */
normalize(vec3);

/**
 * @brief This function calculates the dot product of two vectors.
 *
 * @param vecA The first vector.
 * @param vecB The second vector.
 * @return The one dimensional scalar product.
 */
dot(vecA, vecB);

/**
 * @brief This function calculates the cross product of two vectors.
 * @param vecA The first vector.
 * @param vecB The second vector.
 * @return The calculated vector.
 */
cross(vecA, vecB);

/**
 * @brief Gets the object with the given name.
 *
 * Can also give objects from other scenes than the current one.
 * @param scene The scene to look in. This parameter is optional.
 * @param name The name of the object to retrieve.
 */
getObject(scene, name);

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
 * -- Translate on X-Axis in the local space<br>
 * rotate(object, {15,0,0}, "local")<br>
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
getBehaviorVariable(object, behaviorId, "variableName");
setBehaviorVariable(object, behaviorId, "variableName", value);

/**
 * @brief Sets the current gravity.
 *
 * Example:<br>
 * setGravity({0,0,-0.5})
 * @param vec3 A vec3 containing the new gravitational pull.
 */
setGravity(vec3);
getGravity();
getLinearDamping(object);
setLinearDamping(object, damping);
getAngularDamping(object);
setAngularDamping(object, damping);
getLinearFactor(object);
setLinearFactor(object, {x, y, z});
getAngularFactor(object);
setAngularFactor(object, factor);
getMass(object);
setMass(object, mass);
getFriction(object);
setFriction(object, friction);
getRestitution(object);
setRestitution(object, restitution);
clearForces(object);
addCentralForce(object, {x, y, z}, "local");
addTorque(object, {x, y, z}, "local");
getCentralForce(object);
getTorque(object);
isCollisionTest(object);
isCollisionBetween(object1, object2);
getNumCollisions(object);
rayHit(start, end);
getLightColor(object);
getLightRadius(object);
getLightIntensity(object);
getLightShadowQuality(object);
getLightShadowBias(object);
getLightShadowBlur(object);
getLightSpotAngle(object);
getLightSpotExponent(object);
setLightColor(object, {r, g, b});
setLightRadius(object, radius);
setLightIntensity(object, intensity);
setLightShadowQuality(object, quality);
setLightShadowBias(object, bias);
setLightShadowBlur(object, blur);
setLightSpotAngle(object, spotAngle);
setLightSpotExponent(object, exponent);
changeCurrentCamera(object);
getCurrentCamera();
getCameraClearColor(object);
getCameraFov(object);
getCameraNear(object);
getCameraFar(object);
getCameraFogDistance(object);
isCameraOrtho(object);
isCameraFogEnabled(object);
setCameraClearColor(object, {r, g, b});
setCameraFov(object, fov);
setCameraNear(object, near);
setCameraFar(object, far);
setCameraFogDistance(object, fogDistance);
enableCameraOrtho(object, ortho);
enableCameraFog(object, fog);
enableCameraLayer(object, scene);
disableCameraLayer(object);
enableRenderToTexture(object, "textureName", renderWith, renderHeight);
disableRenderToTexture(object);
getProjectedPoint(object, point);
getUnProjectedPoint(object, point);
setText(object , "text");
getText(object);
getTextColor(object);
setTextColor(object, {r, g, b, a});
playSound(object);
pauseSound(object);
stopSound(object);
getSoundGain(object);
setSoundGain(object, gain);
getCurrentAnimation(object);
changeAnimation(object, animationId);
isAnimationOver(object);
getAnimationSpeed(object);
setAnimationSpeed(object, speed);
getCurrentFrame(object);
setCurrentFrame(object, frame);
centerCursor();
hideCursor();
showCursor();
getWindowScale();
getSystemTick();
quit();
dofile("filename.lua");
loadLevel("levelName");
getScene("sceneName");
changeScene(scene);
getCurrentSceneId();
getScenesNumber();


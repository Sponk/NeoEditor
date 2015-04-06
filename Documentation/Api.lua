---
-- Retrieve the current animation ID the given object uses.
--
-- @param object The object which is animated.
-- @return The animation ID as a number.
--
function getCurrentAnimation(object)

---
-- Plays an animation.
--
-- Changes the current animation to the given one. No animation blending is done.
-- @param object The animated object.
-- @param animationId The new animation to play.
--
function changeAnimation(object, animationId)

---
-- Checks if the current animation has finished playing.
--
-- Also returns true for looped animations if the animation finished this frame.
-- @param object The animated object.
-- @return A boolean value.
--
function isAnimationOver(object)

---
-- Retrieves the speed of the current animation.
--
-- TODO: Get data about the speed unit!
-- @param object The animated object.
-- @return The animation speed.
--
function getAnimationSpeed(object)

---
-- Changes the speed of the current animation.
--
-- TODO: Get data about the speed unit!
-- @param object The animated object.
-- @param speed The animation speed.
--
function setAnimationSpeed(object, speed)

---
-- Retrieves the current frame in the current animation.
--
-- @param object The animated object.
-- @return The current frame as a number.
--
function getCurrentFrame(object)

---
-- Jumps to the given frame of the current animation.
--
-- @param object The animated object.
-- @param frame The the frame to jump to.
--
function setCurrentFrame(object, frame)
---
-- Changes the current active camera.
--
-- This change will apply in the next frame and not immediately.
--
-- @param object The new active camera.
--
function changeCurrentCamera(object)

---
-- Retrieves the active camera.
-- @return The active camera.
--
function getCurrentCamera()

---
-- Retrieves the clear color from the given camera.
-- @return The color as a vec3.
--
-- Example:
-- /code
-- color = getCameraClearColor(camera)
-- print("Color: R = " .. color[1] .. " G = " .. color[2] .. " B = " .. color[3])
-- /endcode
--
function getCameraClearColor(object)

---
-- Retrieves the field of view from the given camera.
-- @param object The FOV as a number.
--
function getCameraFov(object)

---
-- Retrieves the distance to the near plane from the given camera.
-- @param object The distance as a number.
--
function getCameraNear(object)

---
-- Retrieves the distance to the far plane from the given camera.
-- @param object The distance as a number.
--
function getCameraFar(object)

---
-- Retrieves the distance to the fog from the given camera.
-- @param object The distance as a number.
--
function getCameraFogDistance(object)

---
-- Checks if the given camera is configured to be orthographic.
-- @param object The camera.
-- @return A boolean.
--
function isCameraOrtho(object)

---
-- Checks if the given camera is configured to render fog.
-- @param object The camera.
-- @return A boolean.
--
function isCameraFogEnabled(object)
function setCameraClearColor(object, {r, g, b})

---
-- Changes the field of view of the given camera.
-- @param object The camera.
-- @param fov The new field of view.
--
function setCameraFov(object, fov)

---
-- Changes the distance to the near plane of the given camera.
-- @param object The camera.
-- @param near The new distance.
--
function setCameraNear(object, near)

---
-- Changes the distance to the far plane of the given camera.
-- @param object The camera.
-- @param far The new distance.
--
function setCameraFar(object, far)

---
-- Changes the distance to the fog rendered by the given camera.
-- @param object The camera.
-- @param fogDistance The new distance.
--
function setCameraFogDistance(object, fogDistance)

---
-- Updates the cameras matrix setup to allow orthographic rendering.
-- @param object The camera.
-- @param ortho A boolean value representing the new state.
--
function enableCameraOrtho(object, ortho)

---
-- Enables fog rendering for the given camera.
-- @param object The camera.
-- @param fog A boolean value representing the new state.
--
function enableCameraFog(object, fog)

---
-- Turns the image that the given scene renders into an overlay over the given camera.
--
-- For rendering the scene as an overlay the default camera in that scene is used.
--
-- @param object The camera.
-- @param scene The scene to overlay.
--
-- /code
--     camera = getObject("MainCamera")
--     guiScene = getScene("GuiScene")
--
--     enableCameraLayer(camera, guiScene)
-- /endcode
--
function enableCameraLayer(object, scene)

---
-- Disables the scene overlay attached to the given camera.
-- @param object The camera with the scene layer.
--
disableCameraLayer(object)

---
-- Enables render to texture for the given camera.
--
-- This function allows you to setup any camera to render to a specific
-- material. For this you specify the texture path to the texture you want to replace.
-- <strong>Attention:</strong> This will _not_ modify the texture on disk!
-- 
-- All surfaces referencing this texture file will be replaced by the rendered image.
--
-- @param object The camera which is used to render the scene.
-- @param texturePath The path to the texture file to replace.
-- @param renderWidth The width of the render target in pixel (does not need to match the actual texture file)
-- @param renderHeight The width of the render target in pixel (does not need to match the actual texture file)
--
-- /code
--  camera = getObject("MainCamera")
--  enableRenderToTexture(camera, "maps/white.png", 1024, 1024)
-- /endcode
--
function enableRenderToTexture(object, texturePath, renderWidth, renderHeight)

---
-- Disables render to texture for the given camera.
-- @param object The camera.
--
function disableRenderToTexture(object)

---
-- Converts global vector to a projected vector inside the given camera viewport.
--
-- @param object The camera.
-- @param point A vec3 containing positional data.
-- @return A vec3 containing the transformed vector.
--
function getProjectedPoint(object, point)

---
-- Converts a projected vector to a global vector using the given camera.
--
-- This conversion uses the camera to determine the global coordinates for a
-- perspectively transformed point. This can be used to generate a ray for use in
-- raytracing as shown in the example.
--
-- /code
-- camera = getObject("MainCamera")
--
-- -- Start the ray at the center of the screen
-- p1 = getUnProjectedPoint(camera, vec3(0.5, 0.5, 0))
-- p2 = getUnProjectedPoint(camera, vec3(0.5, 0.5, 1))
-- 
-- point, object = rayHit(p1, p2)
-- /endcode
--
-- @param object The camera to use.
-- @param point A vec3 containing positional data.
-- @return A vec3 containing the converted data.
--
function getUnProjectedPoint(object, point)

---
-- Creates a 3D vector.
-- @param x The X value
-- @param y The Y value
-- @param z The Z value
-- @return The newly created 3D vector
--
function vec3(float x, float y, float z)

---
-- Calculates the length of a 3D vector.
-- @param vec3 The 3D vector to use.
-- @return The length of the vector.
--
function length(vec3)

---
-- Normalizes a vector
--
-- This function takes the input vector and transforms it
-- to a new vector with the length 1.0f.
--
-- @param vec3 The vector to normalize.
-- @return The normalized vec3.
--
function normalize(vec3)

---
-- This function calculates the dot product of two vectors.
--
-- @param vecA The first vector.
-- @param vecB The second vector.
-- @return The one dimensional scalar product.
--
function dot(vecA, vecB)

---
-- This function calculates the cross product of two vectors.
-- @param vecA The first vector.
-- @param vecB The second vector.
-- @return The calculated vector.
--
function cross(vecA, vecB)

function getBehaviorVariable(object, behaviorId, "variableName")
function setBehaviorVariable(object, behaviorId, "variableName", value)
---
-- Checks, if the given key is currently pressed.
--
-- See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
-- @param The name of the key.
-- @return A boolean value.
--
function isKeyPressed(key)

---
-- Checks, if the given key got pressed in this frame.
--
-- See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
-- @param The name of the key.
-- @return A boolean value.
--
function onKeyDown(key)

---
-- Checks, if the given key got released in this frame.
--
-- See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
-- @param The name of the key.
-- @return A boolean value.
--
function onKeyUp(key)

---
-- Retrieves the current axis value of a joystick or mouse.
--
-- The value is between 0.0 and 1.0 <br>
--
-- See also: http://www.neo-engine.de/wordpress/?wpwtds_article=key-and-axis-literals
-- @param The name of the key.
-- @return A float value.
--
function getAxis(axis)
function getProperty(name)

---
-- Retrieves the current touch position on touch devices.
--
-- Is the same as the mouse cursor position since SDL makes no difference between them.
-- @param The touch device ID
-- @return The touch position.
--
function getTouchPosition(touchId)
function getLastTouchPosition(touchId)
function getTouchPhase(touchId) 
---
-- Loads a specific level file.
-- @param filename The level to load.
-- Example: 
-- /code
-- loadLevel("levels/menu.level")
-- /endcode
--
function loadLevel(filename)

---
-- Retrieves the scene with the given name.
-- @param name The name of the scene.
-- @return The scene.
--
function getScene(name)

---
-- Changes the current scene to the given scene.
--
-- See also: getScene
--
-- @param scene The new scene.
--
function changeScene(scene)

---
-- Retrieves the ID of the current scene.
-- @return A number.
--
function getCurrentSceneId()

---
-- Returns the number of scenes in the current level.
-- @return A number.
--
function getScenesNumber()
---
-- Returns the color of the given light source.
--
-- The color is encoded as a vec3.
-- @param object The light source.
-- @return The color.
--
function getLightColor(object)

---
-- Returns the radius of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightRadius(object)

---
-- Returns the intensity of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightIntensity(object)

---
-- Returns the shadow quality of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightShadowQuality(object)

---
-- Returns the bias of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightShadowBias(object)

---
-- Returns the blur factor of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightShadowBlur(object)

---
-- Returns the spot angle of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightSpotAngle(object)

---
-- Returns the spot exponent of the given light source.
-- @param object The light source.
-- @return A number.
--
function getLightSpotExponent(object)

---
-- Changes the color of the given light source.
-- @param object The light source.
-- @param color A vec3 containing the RGB values.
--
-- /code
-- setLightColor(light, {0.5,0.5,0.5})
-- /endcode
--
function setLightColor(object, color)

---
-- Changes the radius of the given light source.
-- @param object The light source.
-- @param radius A number.
--
function setLightRadius(object, radius)

---
-- Changes the intensity of the given light source.
-- @param object The light source.
-- @param intensity A number.
--
function setLightIntensity(object, intensity)

---
-- Changes the shadow quality of the given light source.
--
-- The shadow quality is basically the resolution of the texture
-- the depth data will be rendered to. The resolution looks like "quality x quality"
-- and works best if quality is a power of two like 1024 or 2048.
--
-- @param object The light source.
-- @param quality A number. Works best with a power of two.
--
function setLightShadowQuality(object, quality)

---
-- Changes the shadow bias of the given light source.
-- @param object The light source.
-- @param bias A number.
--
function setLightShadowBias(object, bias)

---
-- Changes the shadow blur of the given light source.
--
-- The shadow blur describes the number of passes the GLSL shader
-- uses to blur the shadow edge. Bigger values might slow down your game!
--
-- @param object The light source.
-- @param blur A number. Keep this as small as possible!
--
function setLightShadowBlur(object, blur)

---
-- Changes the spot angle of the given light source.
--
-- The spot angle describes the angle a spot light is able to illuminate.
-- A point light has an angle > 180° and can be turned into a spot light
-- by setting a smaller spot angle.
--
-- @param object The light source.
-- @param spotAngle A number.
--
function setLightSpotAngle(object, spotAngle)

---
-- Changes the spot exponent of the given light source.
--
-- The spot exponent describes the strength of blending out the
-- illuminated area of the spot light on the edges. 
--
-- @param object The light source.
-- @param exponent A number.
--
function setLightSpotExponent(object, exponent)
---
-- Gets the object with the given name.
--
-- Can also give objects from other scenes than the current one.
-- @param scene The scene to look in. This parameter is optional.
-- @param name The name of the object to retrieve.
--
function getObject(scene, name)

---
-- Retrieves the parent of an object.
-- @param The object to retrieve the parent from. See also: getObject
-- @return The parent object.
--
function getParent(object)

---
-- Returns a list of all childrens from the given object.
-- @param object The parent object.
-- @return The list of all children.
--
function getChilds(object)

---
-- Creates a clone of the given object.
-- @param object The object to clone.
-- @return The clone.
--
function getClone(object)

---
-- Changes the parent of the given object.
--
-- Resets parent-child relationship if parent = nil.
-- @param object The object.
-- @param parent The new parent.
--
function setParent(object, parent)

---
-- Retrieves the name of the given object.
-- @param object The object.
-- @return The name of the object.
--
function getName(object)

---
-- Activates the given object.
-- @param object The object.
--
function activate(object)

---
-- Deactivates the given object.
--
-- This function does <strong>not</strong> delete the object from memory!
-- It will just set invisible and removed from the physics simulation.
-- @param object The object.
--
function deactivate(object)

---
-- Checks if the given object is visible.
-- @param object The object.
-- @return A boolean value.
--
function isVisible(object)

---
-- Checks if the given object is active.
-- @param object The object.
-- @return A boolean value.
--
function isActive(object)

---
-- Specifies, if a light renders shadows or if an entity has a shadow.
-- @param object The entity/light.
-- @param shadow A boolean value.
--
function enableShadow(object, shadow)

---
-- Checks if a light renders shadows or if an entity has a shadow.
-- @param object The entity/light.
-- @return A boolean value.
--
function isCastingShadow(object) 
---
-- Sets the current gravity.
--
-- Example:<br>
-- /code
--  setGravity({0,0,-0.5})
-- /endcode
-- @param vec3 A vec3 containing the new gravitational pull.
--
function setGravity(vec3)

---
-- Returns the gravity in the current scene.
-- @return A vec3 containing the gravitational pull.
--
function getGravity()

---
-- Returns the linear damping from the given object.
-- @param object The physically active object
-- @return A number containing the linear damping.
--
function getLinearDamping(object)

---
-- Sets the linear damping of the given object.
-- @param object The physically active object
-- @param damping A number containing the linear damping.
--
function setLinearDamping(object, damping)

---
-- Returns the angular damping from the given object.
-- @param object The physically active object 
-- @return A number containing the angular damping.
--
function getAngularDamping(object)

---
-- Sets the angular damping of the given object.
-- @param object The physically active object
-- @param damping A number containing the angular damping.
--
function setAngularDamping(object, damping)

---
-- Returns the linear factor from the given object.
-- @param object The physically active object
-- @return A vec3 containing the linear factor.
--
function getLinearFactor(object)

---
-- Sets the linear factor of the given object.
-- @param object The physically active object
-- @param factor A vec3 containing the linear factor.
--
function setLinearFactor(object, factor)

---
-- Returns the angular factor from the given object.
-- @return A number containing the linear factor.
--
function getAngularFactor(object)

---
-- Sets the angular factor of the given object.
-- @param object The physically active object
-- @param factor A number containing the angular factor.
--
function setAngularFactor(object, factor)

---
-- Returns the mass of the given object.
-- @param object The physically active object.
-- @return A number.
--
function getMass(object)

---
-- Sets the mass of the given object.
-- @param object The physically active object.
-- @param mass The new mass.
--
function setMass(object, mass)

---
-- Returns the friction constant of the given object.
-- @param object The physically active object.
-- @return A number.
--
function getFriction(object)

---
-- Sets the friction constant of the given object.
-- @param object The physically active object.
-- @param friction The new friction.
--
function setFriction(object, friction)

---
-- Returns the physical restitution of the given object.
-- @param object The physically active object.
-- @return A number.
--
function getRestitution(object)

---
-- Sets the restitution of the given object.
-- @param object The physically active object.
-- @param restitution The new restitution.
--
function setRestitution(object, restitution)

---
-- Removes all active forces from the given object.
-- @param object The physically activated object.
--
function clearForces(object)

---
-- Applies a new central force to the given object.
--
-- The new force will be added to the currently existing central force of the
-- object. The mode parameter is optional and allows you to apply forces relative to the objects
-- current rotation.
--
-- @param object The physically active object.
-- @param force A vec3 containing the force to apply.
-- @param mode A string containing "local". Can be left out.
--
function addCentralForce(object, force, mode)

---
-- Applies a new torque to the given object.
--
-- The new torque will be added to the currently existing torque of the
-- object. The mode parameter is optional and allows you to apply a torque relative to the objects
-- current rotation.
--
-- @param object The physically active object.
-- @param torque A vec3 containing the torque to apply.
-- @param mode A string containing "local". Can be left out.
--
function addTorque(object, torque, mode)

---
-- Returns the force applied to the given object.
-- @param object The physically active object.
-- @return A vec3.
--
function getCentralForce(object)

---
-- Returns the torque applied to the given object.
-- @param object The physically active object.
-- @return A vec3.
--
function getTorque(object)

---
-- Checks if the object is colliding at all.
-- @param object The physically active object.
-- @return A boolean.
--
function isCollisionTest(object)

---
-- Checks if the two given objects are colliding.
-- @param object1 The physically active object.
-- @param object2 The physically active object.
-- @return A boolean.
--
function isCollisionBetween(object1, object2)

---
-- Returns the total number of collisions the given object currently has.
-- @param object The physically active object.
-- @return A number.
--
function getNumCollisions(object)
function rayHit(start, end)---
-- Starts playing the given sound.
-- 
-- @param object A sound object.
--
function playSound(object)

---
-- Pauses the given sound.
--
-- Playback can be resumed by using playSound.
-- @param object A sound object.
--
function pauseSound(object)

---
-- Stops the given sound.
--
-- Playback can be restarted by using playSound.
-- @param object A sound object.
--
function stopSound(object)

---
-- Retrieves the sound volume from the given sound object.
-- @param object A sound object.
-- @return A number.
--
function getSoundGain(object)

---
-- Sets the sound volume of the given sound object.
--
-- The volume level is should be between 0.0 and 1.0 to prevent distortion.
-- @param object A sound object.
-- @param gain A number between 0.0 and 1.0
--
function setSoundGain(object, gain)

---
-- Sets the cursor to the center of the screen.
--
function centerCursor()

---
-- Hides the cursor.
--
function hideCursor()

---
-- Shows the cursor.
--
function showCursor()

---
-- Retrieves the current window resolution.
-- @return The resolution as a vec2.
--
-- Example:
-- /code
-- resolution = getWindowScale()
-- print("The window has a resolution of " .. resolution[1] .. "x" .. resolution[2])
-- /endcode
--
function getWindowScale()

---
-- Returns the number of milliseconds that passed since the application started.
-- @return The current tick count.
--
function getSystemTick()

---
-- Quits the game.
--
-- The player will terminate, the editor will return to an editing environment.
--
function quit()

---
-- Includes a file into script for execution.
-- @param file The file to execute.
--
-- Will look for scripts in "$PROJECT_DIR/scripts".
-- The example would load "$PROJECT_DIR/scripts/filename.lua".
--
-- Example:
-- /code dofile("filename.lua") /endcode
--
function dofile(file)

---
-- Changes the string displayed by a text object.
-- @param object The text object.
-- @param text The new string.
--
-- Example:
-- /code setText(textobject, "Sometext!")/endcode
--
function setText(object , text)

---
-- Retrieves the string displayed by a text object.
-- @param object The text object.
-- @return The current string.
--
function getText(object)

---
-- Retrieves the color displayed by a text object.
-- @param object The text object.
-- @return The current color as a vec4.
--
-- Example:
-- /code
-- color = getTextColor(object)
-- print("Color: R = " .. color[1] .. " G = " .. color[2] .. " B = " .. color[3] .. " A = " .. color[4])
-- /endcode
--
function getTextColor(object)

---
-- Sets the color displayed by a text object.
-- @param object The text object.
-- @param color The new color as a vec4.
--
-- Example:
-- /code
-- setTextColor(object, {r,g,b,a})
-- /endcode
--
function setTextColor(object, color)

---
-- Rotates an object.
--
-- The local parameter is optional.
-- Example:
-- /code
-- -- Rotate around X-Axis
-- rotate(object, {1,0,0}, 90)
-- /endcode
--
-- @param object The object to rotate.
-- @param axis An vec3 containing the axis to rotate around.
-- @param angle The angle to rotate.
-- @param local Specifies if the rotation is in the global or the local space.
--
function rotate(object, axis, angle, "local")

---
-- Translates an object.
--
-- The local parameter is optional.
-- Example:
--
-- /code
-- -- Translate on X-Axis in the local space<br>
-- rotate(object, {15,0,0}, "local")<br>
-- /endcode
--
-- @param object The object to translate.
-- @param axis An vec3 containing the direction to translate to.
-- @param local Specifies if the translation is in the global or the local space.
--
function translate(object, {x, y, z}, "local")

---
-- Retrieves the position of an object.
-- @param object The object.
-- @return A vec3 containing the current position.
--
function getPosition(object)

---
-- Retrieves the rotation of an object.
-- @param object The object.
-- @return A vec3 containing the current rotation.
--
function getRotation(object)

---
-- Retrieves the scale of an object.
-- @param object The object.
-- @return A vec3 containing the current scale.
--
function getScale(object)

---
-- Sets the position of an object.
-- @param object The object.
-- @param pos A vec3 containing the new position.
--
function setPosition(object, pos)

---
-- Sets the rotation of an object.
-- @param object The object.
-- @param rot A vec3 containing the new rotation.
--
function setRotation(object, rot)

---
-- Sets the scale of an object.
-- @param object The object.
-- @param scale A vec3 containing the new scale.
--
function setScale(object, scale)

---
-- Retrieves the transformed position of an object.
--
-- This includes all transformations done by parents.
--
-- @param object The object.
-- @return A vec3 containing the current position.
--
function getTransformedPosition(object)

---
-- Retrieves the transformed rotation of an object.
--
-- This includes all transformations done by parents.
--
-- @param object The object.
-- @return A vec3 containing the current rotation.
--
function getTransformedRotation(object)

---
-- Retrieves the transformed scale of an object.
--
-- This includes all transformations done by parents.
--
-- @param object The object.
-- @return A vec3 containing the current scale.
--
function getTransformedScale(object)

---
-- Calculates the inverse rotated vector according to the matrix of the object.
--
-- @param object The object.
-- @param vector The vec3 containing positional data.
-- @return A vec3 with the transformed data.
--
function getInverseRotatedVector(object, vector)

---
-- Rotates the given vector according to the matrix of the given object.
--
-- @param object The object.
-- @param vector The vec3 containing positional data.
-- @return A vec3 with the transformed data.
--
function getRotatedVector(object, vector)

---
-- Calculates the inverse vector according to the matrix of the object.
--
-- This can be useful for converting global vectors into local vectors.
--
-- @param object The object.
-- @param vector The vec3 containing positional data.
-- @return A vec3 with the transformed data.
--
function getInverseVector(object, vector)

---
-- Multiplies the given with the matrix of the given object.
--
-- This is useful for converting local to global coordinates.
-- @param object The object.
-- @param vector The vec3 containing positional data.
-- @return A vec3 with the transformed data.
--
function getTransformedVector(object, vector)

---
-- Recalculates the matrix of the given object.
-- @param object The object.
--
function updateMatrix(object)

---
-- Retrieves the matrix of an object as a table.
--
-- @param object The object.
-- @return A table containing the current matrix.
--
function getMatrix(object)

---
-- Checks if the object with the given name exists.
-- @param name The name of the object as a string
-- @return A boolean.
function objectExists(name)

---
-- Loads a skybox from the given path.
-- The textures need to be in a directory and need to be named
-- in the fashion of "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg",
-- "posz.jpg", "negz.jpg"
--
-- @param camera The camera object.
-- @param path The path to the directory containing textures
function loadCameraSkybox(camera, path)

---
-- Deletes the given object from the scene and
-- free's its memory.
--
-- <strong>Attention:</strong> Do not use objects after deleting them!
-- @param object The object to delete
function deleteObject(object)

---
-- Resizes the game window.
-- @param width The new width
-- @param height The new height
function resizeWindow(width, height)

---
-- Retrieves the filename of the mesh from the given entity.
-- @param An entity
-- @return A string containing the path
function getMeshFilename(object)

---
-- Loads a mesh from disk and adds it as an entity to the current
-- scene.
-- @param filename Path to the mesh file.
-- @return The new object
function loadMesh(filename)

---
-- Retrieves the object type of the given object
--
-- Valid return values are
-- "Entity", "Camera", "Light", "Sound", "Text", "Object"
-- @return The type as a string
function getObjectType(object)

---
-- Returns the maximum point of the given entity as a vec3.
-- @param object The object
-- @return The point in space
function getBoundingMax(object)

---
-- Returns the minimum point of the given entity as a vec3.
-- @param object The object
-- @return The point in space
function getBoundingMin(object)

---
-- Creates an empty group.
-- Groups have the type "Object" when using "getObjectType(object)"
-- @return The empty group
function createGroup()

---
-- Returns the number of behaviors an object has attached.
-- If the operation fails it returns '-1'.
-- @param object The object to operate on.
-- @return The number of behaviors
function getBehaviorsNumber(object)

---
-- Returns the name of a behavior that is attached to the given object.
-- @param object The object.
-- @param idx The index of the behavior.
-- @return The name as a string.
function getBehaviorName(object, idx)

---
-- Returns the number of variables a behavior owns.
-- @param object The object.
-- @param idx The index of the behavior.
-- @return The count as a number.
function getBehaviorVariablesNumber(object, idx)

---
-- Returns the variable type of a variable that belongs to a behavior
-- as a string.
-- Valid values are:
-- "bool", "vec2", "vec3", "vec4", "float", "int", "string", "uint"
-- @param object The object to operate on
-- @param bid The behavior ID
-- @param vid The variable ID
-- @return The type as a string
function getBehaviorVariableType(object, bid, vid)

---
-- Adds a new behavior to the given object.
-- @param object The object.
-- @param behaviorName The name of the behavior to add.
function addBehavior(object, behaviorName)

---
-- Enables or disables all physics properties of an object.
-- @param object The object to operate on.
-- @param enabled A boolean.
function enablePhysics(object, enabled)

---
-- Sets the physical mass of an object.
-- @param object The entity.
-- @param mass The new mass as a number.
function setMass(object, mass)

---
-- Returns the physical mass of an object.
-- @param object The entity.
-- @return The mass as a number.
function getMass(object)

---
-- Returns the physical friction of an entity.
-- @param object The entity.
-- @return The friction as a number.
function getFriction(object)

---
-- Sets the physical friction of an entity.
-- @param object The entity
-- @param friction The new friction as a number.
function setFriction(object, friction)

---
-- Returns the physical restitution of an object.
-- @param object The entity
-- @return The restitution as a number.
function getRestitution()

---
-- Sets the physical restitution of an entity.
-- @param object The entity
-- @param rest The new restitution as a number.
function setRestitution(object, rest)

---
-- Returns the physical angular factor of an object.
-- @param object The entity
-- @return The angular factor as a number.
function getAngularFactor(object)

---
-- Sets the physical angular factor of an entity.
-- @param object The entity
-- @param value The new angular factor as a number.
function setAngularFactor(object, value)

---
-- Returns the physical linear factor of an object.
-- @param object The entity
-- @return The linear factor as a number.
function getLinearFactor()

---
-- Sets the physical linear factor of an entity.
-- @param object The entity
-- @param value The new linear factor as a number.
function setLinearFactor(object, value)

---
-- Sets the physics quality. The default value is 2, higher values mean
-- higher precision.
-- @param value The new value as a number.
function setPhysicsQuality(value)

---
-- Changes the physical parent of an entity.
-- @param object The entity
-- @param parent The new parent
function setConstraintParent()

---
-- Returns the current physical parent or nil if there is none.
-- @param object The entity to operate on
-- @return The parent as an object.
function getConstraintParent()

---
-- Enables or disables collision with the physical parent object.
-- @param object The entity.
-- @param enabled A boolean value.
function enableParentCollision(object, enabled)

---
-- Returns if the given entity is or is no ghost object.
-- @param object The entity.
-- @return A boolean
function isGhost(object)

---
-- Enables/Disables the ghost property of an entity.
-- @param object The entity
-- @param enabled A boolean
function enableGhost(object, enabled)

---
-- Loads a sound from the given file and adds it to the current scene.
-- @param filename The file to load.
-- @return The new sound object
function loadSound(filename)

---
-- Returns the filename of the given sound object.
-- @param The sound object
-- @return The filename as a string
function getSoundFilename(sound)

---
-- Returns the sound pitch of a sound object.
-- @param object The sound object.
-- @return The pitch as a number.
function getSoundPitch(object)

---
-- Changes the sound pitch.
-- @param object The sound object
-- @param value The new pitch
function setSoundPitch(object, value)

---
-- Returns the sound rolloff value
-- @param object The sound object
-- @return The sound rolloff as a number
function getSoundRolloff(object)

---
-- Changes the sound rolloff of a sound
-- @param object The sound object
-- @param value The new rolloff value
function setSoundRolloff(object, value)

---
-- Returns the sound radius of a sound object.
-- @param object The sound object
-- @return The radius as a number
function getSoundRadius(object)

---
-- Changes the sound radius of a sound object.
-- @param object The sound object
-- @param value The new radius as a number
function setSoundRadius(object, value)

---
-- Checks if the sound is relative
-- @param object The sound object
-- @return A boolean
function isSoundRelative(object)

---
-- Sets the sound relative according to the given value
-- @param object The sound object
-- @param enabled A boolean
function setSoundRelative(object, enabled)

---
-- Checks if the sound is looping
-- @param object The sound object
-- @return A boolean
function isSoundLooping(object)

---
-- Sets the sound relative according to the given value
-- @param object The sound object
-- @param enabled A boolean
function setSoundLooping()

---
-- Creates a new light object
-- @return The new light object
function createLight()

---
-- Creates a new camera object
-- @return The new camera
function createCamera()

---
-- Creates a new text object with the given font
-- @param filename The path to the font to use
-- @return A new text object
function loadTextFont(filename)

---
-- Returns the file name of the font used by the given text object.
-- @param object The text object
-- @return the file as a string
function getFontFilename(object)

---
-- Returns the font size of the given text
-- @param object The text object
-- @return The font size as a number
function getTextFontSize(object)

---
-- Changes the font size of the given text
-- @param object The text object
-- @param size The new size as a number
function setTextFontSize(object, size)

---
-- Changes the text alignment
--
-- The alignment is given as a string.
-- Valid values are: "Center", "Left", "Right"
--
-- @param object The text object
-- @param align The new alignment as a string
function setTextAlignment(object, alignment)

---
-- Returns the text alignment
--
-- The alignment is given as a string.
-- Valid values are: "Center", "Left", "Right"
--
-- @param object The text object
-- @return The alignment as a string
function getTextAlignment(object)

---
-- Returns the current working directory
-- @return The working directory as a string
function getWorkingDirectory()

---
-- Loads a text file completely from disk. Also accesses files inside
-- the game NPK after the game is published.
-- @param filename The file to load.
-- @return The content as a string
function loadTextFile(filename)

---
-- Checks if a file exists
-- @param filename The file to check
-- @return A boolean
function isFileExist(filename)

---
-- Creates a new directory.
-- @param path The directory to create.
function createDirectory(path)


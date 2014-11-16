/**
 * @brief Changes the current active camera.
 *
 * This change will apply in the next frame and not immediately.
 *
 * @param object The new active camera.
 */
changeCurrentCamera(object);

/**
 * @brief Retrieves the active camera.
 * @return The active camera.
 */
getCurrentCamera();

/**
 * @brief Retrieves the clear color from the given camera.
 * @return The color as a vec3.
 *
 * Example:
 * @code
 * color = getCameraClearColor(camera)
 * print("Color: R = " .. color[1] .. " G = " .. color[2] .. " B = " .. color[3])
 * @endcode
 */
getCameraClearColor(object);

/**
 * @brief Retrieves the field of view from the given camera.
 * @param object The FOV as a number.
 */
getCameraFov(object);

/**
 * @brief Retrieves the distance to the near plane from the given camera.
 * @param object The distance as a number.
 */
getCameraNear(object);

/**
 * @brief Retrieves the distance to the far plane from the given camera.
 * @param object The distance as a number.
 */
getCameraFar(object);

/**
 * @brief Retrieves the distance to the fog from the given camera.
 * @param object The distance as a number.
 */
getCameraFogDistance(object);

/**
 * @brief Checks if the given camera is configured to be orthographic.
 * @param object The camera.
 * @return A boolean.
 */
isCameraOrtho(object);

/**
 * @brief Checks if the given camera is configured to render fog.
 * @param object The camera.
 * @return A boolean.
 */
isCameraFogEnabled(object);
setCameraClearColor(object, {r, g, b});

/**
 * @brief Changes the field of view of the given camera.
 * @param object The camera.
 * @param fov The new field of view.
 */
setCameraFov(object, fov);

/**
 * @brief Changes the distance to the near plane of the given camera.
 * @param object The camera.
 * @param near The new distance.
 */
setCameraNear(object, near);

/**
 * @brief Changes the distance to the far plane of the given camera.
 * @param object The camera.
 * @param far The new distance.
 */
setCameraFar(object, far);

/**
 * @brief Changes the distance to the fog rendered by the given camera.
 * @param object The camera.
 * @param fogDistance The new distance.
 */
setCameraFogDistance(object, fogDistance);

/**
 * @brief Updates the cameras matrix setup to allow orthographic rendering.
 * @param object The camera.
 * @param ortho A boolean value representing the new state.
 */
enableCameraOrtho(object, ortho);

/**
 * @brief Enables fog rendering for the given camera.
 * @param object The camera.
 * @param fog A boolean value representing the new state.
 */
enableCameraFog(object, fog);

/**
 * @brief Turns the image that the given scene renders into an overlay over the given camera.
 *
 * For rendering the scene as an overlay the default camera in that scene is used.
 *
 * @param object The camera.
 * @param scene The scene to overlay.
 *
 * @code
 * camera = getObject("MainCamera")
 * guiScene = getScene("GuiScene")
 *
 * enableCameraLayer(camera, guiScene)
 * @endcode
 */
enableCameraLayer(object, scene);

/**
 * @brief Disables the scene overlay attached to the given camera.
 * @param object The camera with the scene layer.
 */
disableCameraLayer(object);

/**
 * @brief Enables render to texture for the given camera.
 *
 * This function allows you to setup any camera to render to a specific
 * material. For this you specify the texture path to the texture you want to replace.
 * <strong>Attention:</strong> This will _not_ modify the texture on disk!
 * 
 * All surfaces referencing this texture file will be replaced by the rendered image.
 *
 * @param object The camera which is used to render the scene.
 * @param texturePath The path to the texture file to replace.
 * @param renderWidth The width of the render target in pixel (does not need to match the actual texture file)
 * @param renderHeight The width of the render target in pixel (does not need to match the actual texture file)
 *
 * @code
 * camera = getObject("MainCamera")
 * enableRenderToTexture(camera, "maps/white.png", 1024, 1024)
 * @endcode
 */
enableRenderToTexture(object, texturePath, renderWidth, renderHeight);

/**
 * @brief Disables render to texture for the given camera.
 * @param object The camera.
 */
disableRenderToTexture(object);
getProjectedPoint(object, point);
getUnProjectedPoint(object, point);

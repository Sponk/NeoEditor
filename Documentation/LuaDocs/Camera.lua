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

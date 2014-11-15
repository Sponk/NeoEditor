/**
 * @brief Loads a specific level file.
 * @param filename The level to load.
 * Example: 
 * @startcode
 * loadLevel("levels/menu.level")
 * @endcode
 */
loadLevel(filename);

/**
 * @brief Retrieves the scene with the given name.
 * @param name The name of the scene.
 * @return The scene.
 */
getScene(name);

/**
 * @brief Changes the current scene to the given scene.
 *
 * See also: getScene
 *
 * @param scene The new scene.
 */
changeScene(scene);

/**
 * @brief Retrieves the ID of the current scene.
 * @return A number.
 */
getCurrentSceneId();

/**
 * @brief Returns the number of scenes in the current level.
 * @return A number.
 */
getScenesNumber();

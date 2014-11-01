/**
 * @brief Sets the cursor to the center of the screen.
 */
centerCursor();

/**
 * @brief Hides the cursor.
 */
hideCursor();

/**
 * @brief Shows the cursor.
 */
showCursor();

/**
 * @brief Retrieves the current window resolution.
 * @return The resolution as a vec2.
 *
 * Example:
 * @code
 * resolution = getWindowScale()
 * print("The window has a resolution of " .. resolution[1] .. "x" .. resolution[2])
 * @endcode
 */
getWindowScale();

/**
 * @brief Returns the number of milliseconds that passed since the application started.
 * @return The current tick count.
 */
getSystemTick();

/**
 * @brief Quits the game.
 *
 * The player will terminate, the editor will return to an editing environment.
 */
quit();

/**
 * @brief Includes a file into script for execution.
 * @param file The file to execute.
 *
 * Will look for scripts in "$PROJECT_DIR/scripts".
 * The example would load "$PROJECT_DIR/scripts/filename.lua".
 *
 * Example:
 * @code dofile("filename.lua") @endcode
 */
dofile(file);

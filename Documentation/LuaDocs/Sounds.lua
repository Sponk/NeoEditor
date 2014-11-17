/**
 * @brief Starts playing the given sound.
 * 
 * @param object A sound object.
 */
playSound(object);

/**
 * @brief Pauses the given sound.
 *
 * Playback can be resumed by using playSound.
 * @param object A sound object.
 */
pauseSound(object);

/**
 * @brief Stops the given sound.
 *
 * Playback can be restarted by using playSound.
 * @param object A sound object.
 */
stopSound(object);

/**
 * @brief Retrieves the sound volume from the given sound object.
 * @param object A sound object.
 * @return A number.
 */
 
getSoundGain(object);

/**
 * @brief Sets the sound volume of the given sound object.
 *
 * The volume level is should be between 0.0 and 1.0 to prevent distortion.
 * @param object A sound object.
 * @param gain A number between 0.0 and 1.0
 */
setSoundGain(object, gain);
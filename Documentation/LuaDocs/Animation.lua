/**
 * @brief Retrieve the current animation ID the given object uses.
 *
 * @param object The object which is animated.
 * @return The animation ID as a number.
 */
getCurrentAnimation(object);

/**
 * @brief Plays an animation.
 *
 * Changes the current animation to the given one. No animation blending is done.
 * @param object The animated object.
 * @param animationId The new animation to play.
 */
changeAnimation(object, animationId);

/**
 * @brief Checks if the current animation has finished playing.
 *
 * Also returns true for looped animations if the animation finished this frame.
 * @param object The animated object.
 * @return A boolean value.
 */
isAnimationOver(object);

/**
 * @brief Retrieves the speed of the current animation.
 *
 * TODO: Get data about the speed unit!
 * @param object The animated object.
 * @return The animation speed.
 */
getAnimationSpeed(object);

/**
 * @brief Changes the speed of the current animation.
 *
 * TODO: Get data about the speed unit!
 * @param object The animated object.
 * @param speed The animation speed.
 */
setAnimationSpeed(object, speed);

/**
 * @brief Retrieves the current frame in the current animation.
 *
 * @param object The animated object.
 * @return The current frame as a number.
 */
getCurrentFrame(object);

/**
 * @brief Jumps to the given frame of the current animation.
 *
 * @param object The animated object.
 * @param frame The the frame to jump to.
 */
setCurrentFrame(object, frame);

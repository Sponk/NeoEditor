
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

getBehaviorVariable(object, behaviorId, "variableName");
setBehaviorVariable(object, behaviorId, "variableName", value);

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
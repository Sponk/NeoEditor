//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifndef __PHYSICS_CONTEXT_H
#define __PHYSICS_CONTEXT_H

namespace Neo
{
/**
 * @brief The PhysicsContext class contains abstraction to 3D physics functionality.
 */
class NEO_CORE_EXPORT PhysicsContext
{
public:

	// destructor
	virtual ~PhysicsContext(void){}

	// init
	virtual void init(const Vector3 & worldMin, const Vector3 & worldMax) = 0;

	// destroy
	virtual void clear(void) = 0;

	// update simulation

    /**
     * @brief Sets the simulation quality
     *
     * Default simulation quality is 2 when using Bullet. Any value lower than two will result in worse,
     * any value higher in better physics quality. Higher values cost more time to
     * calculate so keep the quality as low as possible!
     *
     * @param quality The quality level. Defaults to 2 when using Bullet
     */
	virtual void setSimulationQuality(unsigned int quality) = 0;
	virtual void updateSimulation(void) = 0;

	// world
    /**
     * @brief Sets the world gravity.
     * @param gravity An MVector3 containing the new gravitational pull to be used for example MVector3(0.0f, 0.0f, -9.81)
     */
	virtual void setWorldGravity(const Vector3 & gravity) = 0;

	// create object
	virtual void createGhost(unsigned int * objectId, unsigned int shapeId, const Vector3 & position, const Quaternion & rotation) = 0;
	virtual void createRigidBody(unsigned int * objectId, unsigned int shapeId, const Vector3 & position, const Quaternion & rotation, float mass) = 0;

	// activate / deactivate
	virtual void activateObject(unsigned int objectId) = 0;
	virtual void deactivateObject(unsigned int objectId) = 0;
	
	// delete object
	virtual void deleteObject(unsigned int * objectId) = 0;

	// object properties
	virtual void enableObjectKinematic(unsigned int objectId) = 0;
	virtual void disableObjectKinematic(unsigned int objectId) = 0;

	virtual void setObjectShape(unsigned int objectId, unsigned int shapeId) = 0;
	virtual void setObjectMass(unsigned int objectId, float mass) = 0;
	virtual void setObjectRestitution(unsigned int objectId, float restitution) = 0;
	virtual void setObjectFriction(unsigned int objectId, float friction) = 0;
	virtual void setObjectLinearFactor(unsigned int objectId, const Vector3 & linearFactor) = 0;
	virtual void setObjectAngularFactor(unsigned int objectId, float angularFactor) = 0;
	virtual void setObjectDamping(unsigned int objectId, float linearDamping, float angularDamping) = 0;
	virtual void setObjectTransform(unsigned int objectId, const Vector3 & position, const Quaternion & rotation) = 0;
	virtual void getObjectTransform(unsigned int objectId, Vector3 * position, Quaternion * rotation) = 0;

	virtual void setObjectUserPointer(unsigned int objectId, void * userPointer) = 0;
	virtual void * getObjectUserPointer(unsigned int objectId) = 0;
	
	// affectors
	virtual void addCentralForce(unsigned int objectId, const Vector3 & force) = 0;
	virtual void getCentralForce(unsigned int objectId, Vector3 * force) = 0;
	virtual void addTorque(unsigned int objectId, const Vector3 & torque) = 0;
	virtual void getTorque(unsigned int objectId, Vector3 * torque) = 0;
	virtual void clearForces(unsigned int objectId) = 0;

	// objects collision
	virtual int isObjectInCollision(unsigned int objectId, unsigned int * collisionList = NULL, unsigned int size = 0) = 0;
	virtual bool isObjectsCollision(unsigned int object1Id, unsigned int object2Id) = 0;
	virtual bool isRayHit(const Vector3 & start, const Vector3 & end, unsigned int * objectId = NULL, Vector3 * point = NULL, Vector3 * normal = NULL) = 0;

	// create shape
	virtual void createMultiShape(unsigned int * shapeId) = 0;

	virtual void createBoxShape(unsigned int * shapeId, const Vector3 & scale) = 0;
	virtual void createSphereShape(unsigned int * shapeId, float radius) = 0;
	virtual void createConeShape(unsigned int * shapeId, float radius, float height) = 0;
	virtual void createCapsuleShape(unsigned int * shapeId, float radius, float height) = 0;
	virtual void createCylinderShape(unsigned int * shapeId, float radius, float height) = 0;

	virtual void createConvexHullShape(unsigned int * shapeId, const Vector3 * vertices, unsigned int verticesNumber, const Vector3 scale = Vector3(1)) = 0;
	virtual void createTriangleMeshShape(unsigned int * shapeId, const Vector3 * vertices, unsigned int verticesNumber, const void * indices, unsigned int indicesNumber, VAR_TYPES indicesType, const Vector3 scale = Vector3(1)) = 0;

	// delete shape
	virtual void deleteShape(unsigned int * shapeId) = 0;

	// add child shape to multishape
	virtual void addChildShape(unsigned int multiShapeId, unsigned int shapeId, const Vector3 & position, const Quaternion & rotation) = 0;

	// create constraint
	virtual void createConstraint(unsigned int * constraintId, unsigned int parentObjectId, unsigned int objectId, const Vector3 & pivot, bool disableParentCollision) = 0;
	virtual void setLinearLimit(unsigned int constraintId, const Vector3 & lower, const Vector3 & upper) = 0;
	virtual void setAngularLimit(unsigned int constraintId, const Vector3 & lower, const Vector3 & upper) = 0;
	
	// delete constraint
	virtual void deleteConstraint(unsigned int * constraintId) = 0;
};

class NEO_CORE_EXPORT PhysicsContextDummy : public PhysicsContext
{
public:

	virtual ~PhysicsContextDummy() {}

	virtual void init(const Vector3 & worldMin, const Vector3 & worldMax) {}
	virtual void clear(void) {}
	virtual void setSimulationQuality(unsigned int quality) {}
	virtual void updateSimulation(void) {}

	virtual void setWorldGravity(const Vector3 & gravity) {}
	virtual void createGhost(unsigned int * objectId, unsigned int shapeId, const Vector3 & position, const Quaternion & rotation) {}
	virtual void createRigidBody(unsigned int * objectId, unsigned int shapeId, const Vector3 & position, const Quaternion & rotation, float mass) {}
	virtual void activateObject(unsigned int objectId) {}
	virtual void deactivateObject(unsigned int objectId) {}
	virtual void deleteObject(unsigned int * objectId) {}
	virtual void enableObjectKinematic(unsigned int objectId) {}
	virtual void disableObjectKinematic(unsigned int objectId) {}

	virtual void setObjectShape(unsigned int objectId, unsigned int shapeId) {}
	virtual void setObjectMass(unsigned int objectId, float mass) {}
	virtual void setObjectRestitution(unsigned int objectId, float restitution) {}
	virtual void setObjectFriction(unsigned int objectId, float friction) {}
	virtual void setObjectLinearFactor(unsigned int objectId, const Vector3 & linearFactor) {}
	virtual void setObjectAngularFactor(unsigned int objectId, float angularFactor) {}
	virtual void setObjectDamping(unsigned int objectId, float linearDamping, float angularDamping) {}
	virtual void setObjectTransform(unsigned int objectId, const Vector3 & position, const Quaternion & rotation) {}
	virtual void getObjectTransform(unsigned int objectId, Vector3 * position, Quaternion * rotation) {}

	virtual void setObjectUserPointer(unsigned int objectId, void * userPointer) {}
	virtual void * getObjectUserPointer(unsigned int objectId) { return NULL; }

	virtual void addCentralForce(unsigned int objectId, const Vector3 & force) {}
	virtual void getCentralForce(unsigned int objectId, Vector3 * force) {}
	virtual void addTorque(unsigned int objectId, const Vector3 & torque) {}
	virtual void getTorque(unsigned int objectId, Vector3 * torque) {}
	virtual void clearForces(unsigned int objectId) {}

	virtual int isObjectInCollision(unsigned int objectId, unsigned int * collisionList = NULL, unsigned int size = 0) { return false; }
	virtual bool isObjectsCollision(unsigned int object1Id, unsigned int object2Id) { return false; }
	virtual bool isRayHit(const Vector3 & start, const Vector3 & end, unsigned int * objectId = NULL, Vector3 * point = NULL, Vector3 * normal = NULL) { return false; }

	virtual void createMultiShape(unsigned int * shapeId) {}
	virtual void createBoxShape(unsigned int * shapeId, const Vector3 & scale) {}
	virtual void createSphereShape(unsigned int * shapeId, float radius) {}
	virtual void createConeShape(unsigned int * shapeId, float radius, float height) {}
	virtual void createCapsuleShape(unsigned int * shapeId, float radius, float height) {}
	virtual void createCylinderShape(unsigned int * shapeId, float radius, float height) {}

	virtual void createConvexHullShape(unsigned int * shapeId, const Vector3 * vertices, unsigned int verticesNumber, const Vector3 scale = Vector3(1)) {}
	virtual void createTriangleMeshShape(unsigned int * shapeId, const Vector3 * vertices, unsigned int verticesNumber, const void * indices, unsigned int indicesNumber, VAR_TYPES indicesType, const Vector3 scale = Vector3(1)) {}

	virtual void deleteShape(unsigned int * shapeId) {}
	virtual void addChildShape(unsigned int multiShapeId, unsigned int shapeId, const Vector3 & position, const Quaternion & rotation) {}

	virtual void createConstraint(unsigned int * constraintId, unsigned int parentObjectId, unsigned int objectId, const Vector3 & pivot, bool disableParentCollision) {}
	virtual void setLinearLimit(unsigned int constraintId, const Vector3 & lower, const Vector3 & upper) {}
	virtual void setAngularLimit(unsigned int constraintId, const Vector3 & lower, const Vector3 & upper) {}
	virtual void deleteConstraint(unsigned int * constraintId) {}
};
}

#endif

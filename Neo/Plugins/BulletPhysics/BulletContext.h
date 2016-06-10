//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef _BULLET_CONTEXT_H
#define _BULLET_CONTEXT_H

#include <NeoCore.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

class BulletContext : public Neo::PhysicsContext
{
	class MBUserData
	{
	public:
		MBUserData(void) : m_objectId(0), m_userPointer(NULL) {}
		unsigned int m_objectId;
		void* m_userPointer;
	};

	unsigned int m_quality;

	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;

	vector<MBUserData*> m_userDatas;
	vector<btCollisionShape*> m_collisionShapes;
	vector<btCollisionObject*> m_collisionObjects;
	vector<btTypedConstraint*> m_constraints;

public:
	// constructor
	BulletContext(void);

	// destructor
	~BulletContext(void);

	// init
	void init(const Neo::Vector3& worldMin, const Neo::Vector3& worldMax);

	// clear
	void clear(void);

	// update simulation
	void setSimulationQuality(unsigned int quality);
	void updateSimulation(void);

	// world gravity
	void setWorldGravity(const Neo::Vector3& gravity);

	// create object
	void createGhost(unsigned int* objectId, unsigned int shapeId,
					 const Neo::Vector3& position, const Neo::Quaternion& rotation);

	void createRigidBody(unsigned int* objectId, unsigned int shapeId,
						 const Neo::Vector3& position, const Neo::Quaternion& rotation,
						 float mass);

	// activate / deactivate
	void activateObject(unsigned int objectId);
	void deactivateObject(unsigned int objectId);

	// delete object
	void deleteObject(unsigned int* objectId);

	// object properties
	void enableObjectKinematic(unsigned int objectId);
	void disableObjectKinematic(unsigned int objectId);

	void setObjectShape(unsigned int objectId, unsigned int shapeId);
	void setObjectMass(unsigned int objectId, float mass);
	void setObjectRestitution(unsigned int objectId, float restitution);
	void setObjectFriction(unsigned int objectId, float friction);

	void setObjectLinearFactor(unsigned int objectId,
							   const Neo::Vector3& linearFactor);

	void setObjectAngularFactor(unsigned int objectId, float angularFactor);

	void setObjectDamping(unsigned int objectId, float linearDamping,
						  float angularDamping);

	void setObjectTransform(unsigned int objectId, const Neo::Vector3& position,
							const Neo::Quaternion& rotation);
	
	void getObjectTransform(unsigned int objectId, Neo::Vector3* position,
							Neo::Quaternion* rotation);

	void setObjectUserPointer(unsigned int objectId, void* userPointer);
	void* getObjectUserPointer(unsigned int objectId);

	// affectors
	void addCentralForce(unsigned int objectId, const Neo::Vector3& force);
	void getCentralForce(unsigned int objectId, Neo::Vector3* force);
	void addTorque(unsigned int objectId, const Neo::Vector3& torque);
	void getTorque(unsigned int objectId, Neo::Vector3* torque);
	void clearForces(unsigned int objectId);

	// objects collision
	int isObjectInCollision(unsigned int objectId,
							unsigned int* collisionList = NULL,
							unsigned int size = 0);
	
	bool isObjectsCollision(unsigned int objectId, unsigned int object2Id);
	bool isRayHit(const Neo::Vector3& start, const Neo::Vector3& end,
				  unsigned int* objectId = NULL, Neo::Vector3* point = NULL,
				  Neo::Vector3* normal = NULL);

	// create shape
	void createMultiShape(unsigned int* shapeId);

	void createBoxShape(unsigned int* shapeId, const Neo::Vector3& scale);
	void createSphereShape(unsigned int* shapeId, float radius);
	void createConeShape(unsigned int* shapeId, float radius, float height);
	void createCapsuleShape(unsigned int* shapeId, float radius, float height);
	void createCylinderShape(unsigned int* shapeId, float radius, float height);

	void createConvexHullShape(unsigned int* shapeId, const Neo::Vector3* vertices,
							   unsigned int verticesNumber,
							   const Neo::Vector3 scale = Neo::Vector3(1));
	
	void createTriangleMeshShape(unsigned int* shapeId, const Neo::Vector3* vertices,
								 unsigned int verticesNumber,
								 const void* indices,
								 unsigned int indicesNumber,
								 Neo::VAR_TYPES indicesType,
								 const Neo::Vector3 scale = Neo::Vector3(1));

	// delete shape
	void deleteShape(unsigned int* shapeId);

	// add child shape to multishape
	void addChildShape(unsigned int multiShapeId, unsigned int shapeId,
					   const Neo::Vector3& position, const Neo::Quaternion& rotation);

	// create constraint
	void createConstraint(unsigned int* constraintId,
						  unsigned int parentObjectId, unsigned int objectId,
						  const Neo::Vector3& pivot, bool disableParentCollision);
	
	void setLinearLimit(unsigned int constraintId, const Neo::Vector3& lower,
						const Neo::Vector3& upper);

	void setAngularLimit(unsigned int constraintId, const Neo::Vector3& lower,
						 const Neo::Vector3& upper);

	// delete constraint
	void deleteConstraint(unsigned int* constraintId);
};

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOEntity.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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


#ifndef _M_OENTITY_H
#define _M_OENTITY_H

namespace Neo
{

// collision shape types
enum COLLISION_SHAPE_TYPE
{
	COLLISION_SHAPE_BOX = 0,
	COLLISION_SHAPE_SPHERE,
	COLLISION_SHAPE_CONE,
	COLLISION_SHAPE_CAPSULE,
	COLLISION_SHAPE_CYLINDER,
	COLLISION_SHAPE_CONVEX_HULL,
	COLLISION_SHAPE_TRIANGLE_MESH
};

struct PhysicsConstraint
{
	PhysicsConstraint(void);
	PhysicsConstraint(const PhysicsConstraint & constraint);
	
	unsigned int constraintId;
	unsigned int parentObjectId;
	
	MString parentName;
	MVector3 pivot;
	MVector3 lowerLinearLimit;
	MVector3 upperLinearLimit;
	MVector3 lowerAngularLimit;
	MVector3 upperAngularLimit;
	bool disableParentCollision;
};

class M_ENGINE_EXPORT PhysicsProperties
{
public:

	PhysicsProperties(void);
	PhysicsProperties(const PhysicsProperties & physicsProperties);
	~PhysicsProperties(void);

private:

	PhysicsConstraint * m_constraint;
	
	unsigned int m_shapeId;
	unsigned int m_collisionObjectId;
	COLLISION_SHAPE_TYPE m_collisionShape;

	bool m_ghost;
	float m_mass;
	float m_friction;
	float m_restitution;
	float m_linearDamping;
	float m_angularDamping;
	float m_angularFactor;
	MVector3 m_linearFactor;

public:

    // constraint
	void deleteConstraint(void);
	PhysicsConstraint * createConstraint(void);
	inline PhysicsConstraint * getConstraint(void){ return m_constraint; }

	// shape id
	inline void setShapeId(unsigned int shapeId){ m_shapeId = shapeId; }
	inline unsigned int getShapeId(void){ return m_shapeId; }

	// collision object id
	inline void setCollisionObjectId(unsigned int collisionObjectId){ m_collisionObjectId = collisionObjectId; }
	inline unsigned int getCollisionObjectId(void){ return m_collisionObjectId; }

	// collision shape
	inline void setCollisionShape(COLLISION_SHAPE_TYPE collisionShape){ m_collisionShape = collisionShape; }
	inline COLLISION_SHAPE_TYPE getCollisionShape(void){ return m_collisionShape; }

	// ghost
	inline void setGhost(bool ghost){ m_ghost = ghost; }
	inline bool isGhost(void){ return m_ghost; }

	// mass
	inline void setMass(float mass){ m_mass = mass; }
	inline float getMass(void){ return m_mass; }

	// friction
	inline void setFriction(float friction){ m_friction = friction; }
	inline float getFriction(void){ return m_friction; }

	// restitution
	inline void setRestitution(float restitution){ m_restitution = restitution; }
	inline float getRestitution(void){ return m_restitution; }

	// linear damping
	inline void setLinearDamping(float linearDamping){ m_linearDamping = linearDamping; }
	inline float getLinearDamping(void){ return m_linearDamping; }

	// angular damping
	inline void setAngularDamping(float angularDamping){ m_angularDamping = angularDamping; }
	inline float getAngularDamping(void){ return m_angularDamping; }

	// angular factor
	inline void setAngularFactor(float angularFactor){ m_angularFactor = angularFactor; }
	inline float getAngularFactor(void){ return m_angularFactor; }

	// linear factor
	inline void setLinearFactor(const MVector3 & linearFactor){ m_linearFactor = linearFactor; }
	inline MVector3 * getLinearFactor(void){ return &m_linearFactor; }
};

class M_ENGINE_EXPORT OEntity : public Object3d
{
public:

	// constructor / destructor
	OEntity(MeshRef * meshRef);
	~OEntity(void);

	// copy constructor
	OEntity(const OEntity & entity);

private:

	// mesh
	MeshRef * m_meshRef;

	// invisible
	bool m_isInvisible;
    bool m_hasShadow;

	// animation
	unsigned int m_animationId;
	int m_currentLoop;
	float m_animationSpeed;
	float m_currentFrame;

	// collision
	PhysicsProperties * m_physicsProperties;

	// bounding box
	Box3d m_boundingBox;
    bool m_isOccluder;

public:

    // occluder
    inline void enableOccluder(bool value) { m_isOccluder = value; }
    inline bool isOccluder() { return m_isOccluder; }

	// type
	int getType(void){ return M_OBJECT3D_ENTITY; }

	void setActive(bool active);
	
	// invisible
	void setInvisible(bool invisible){ m_isInvisible = invisible; }
	bool isInvisible(void){ return m_isInvisible; }

	// mesh
	Mesh * getMesh(void);
	void setMeshRef(MeshRef * meshRef);
	inline MeshRef * getMeshRef(void){ return m_meshRef; }

	// animation
	void changeAnimation(unsigned int animationId);
	bool isAnimationOver(void);
	inline void setAnimationSpeed(float animationSpeed){ m_animationSpeed = animationSpeed; }
	inline void setCurrentFrame(float currentFrame){ m_currentFrame = currentFrame; }
	inline unsigned int getAnimationId(void){ return m_animationId; }
	inline float getAnimationSpeed(void){ return m_animationSpeed; }
	inline float getCurrentFrame(void){ return m_currentFrame; }

	// physics
	void deletePhysicsProperties(void);
	PhysicsProperties * createPhysicsProperties(void);
	inline PhysicsProperties * getPhysicsProperties(void){ return m_physicsProperties; }

	// bounding box
	inline Box3d * getBoundingBox(void){ return &m_boundingBox; }

    // Shadow
    inline bool hasShadow() { return m_hasShadow; }
    inline void enableShadow(bool shadow) { m_hasShadow = shadow; }

	// update
	void update(void);

	// visibility
	void updateVisibility(OCamera * camera);
};
}
#endif

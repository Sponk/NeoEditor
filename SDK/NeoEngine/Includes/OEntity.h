//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __OENTITY_H
#define __OENTITY_H

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

struct NEO_ENGINE_EXPORT PhysicsConstraint
{
	PhysicsConstraint(void);
	PhysicsConstraint(const PhysicsConstraint & constraint);
	
	unsigned int constraintId;
	unsigned int parentObjectId;
	
	String parentName;
	Vector3 pivot;
	Vector3 lowerLinearLimit;
	Vector3 upperLinearLimit;
	Vector3 lowerAngularLimit;
	Vector3 upperAngularLimit;
	bool disableParentCollision;
};

class NEO_ENGINE_EXPORT PhysicsProperties
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
	Vector3 m_linearFactor;

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
	inline void setLinearFactor(const Vector3 & linearFactor){ m_linearFactor = linearFactor; }
	inline Vector3 * getLinearFactor(void){ return &m_linearFactor; }
};

/**
 * @brief Represents a 3D mesh in the scene.
 *
 * How to create a new OEntity
 * @code
 * Level* level = NeoEngine::getInstance()->getLevel();
 * Scene* scene = level->getCurrentScene();
 * OEntity* newEntity = scene->addNewEntity(level->loadMesh("assets/mesh.dae"));
 * @endcode
 * @see Object3d
 */
class NEO_ENGINE_EXPORT OEntity : public Object3d
{
public:
	// constructor / destructor
	OEntity(MeshRef *meshRef);
	~OEntity(void);

	// copy constructor
	OEntity(const OEntity &entity);

private:
	// mesh
	MeshRef *m_meshRef;

	// invisible
	bool m_isInvisible;
	bool m_hasShadow;

	// animation
	unsigned int m_animationId;
	int m_currentLoop;
	float m_animationSpeed;
	float m_currentFrame;

	// collision
	PhysicsProperties *m_physicsProperties;

	// bounding box
	Box3d m_boundingBox;
	bool m_isOccluder;

	bool m_hasTransparency;
	
public:

	bool hasTransparency() { return m_hasTransparency; }

	/**
	 * @brief Sets up the OEntity to become a physical active object
	 * in the given scene.
	 *
	 * @param scene The scene the object operates in.
	 * @return The new PhysicsProperties of the object.
	 */
	PhysicsProperties* enablePhysics(Scene *scene);
	
	/**
	 * @brief Sets up the OEntity to become a physical active object
	 * in the current scene.
	 * @return The new PhysicsProperties of the object.
	 * @see enablePhysics(Scene* scene)
	 */
	PhysicsProperties* enablePhysics();

	/**
	 * @brief Checks if the object is colliding with the other object.
	 * @param entity The other object.
	 * @return A boolean value.
	 * @see enablePhysics
	 * @see createPhysicsProperties
	 */
	bool isColliding(OEntity* entity);

	/**
	 * @brief Checks if the object has any collision with any other object.
	 * @return A boolean value.
	 * @see isColliding(OEntity*)
	 */
	bool isColliding();

	/**
	 * @brief Sets if this object is a occluder or not.
	 *
	 * An occluder is used by the renderer to do occlusion culling.
	 * @param value A boolean value.
	 */
	inline void enableOccluder(bool value) { m_isOccluder = value; }

	/**
	 * @brief Checks if the OEntity is an occluder.
	 * @return A boolean value.
	 */
	inline bool isOccluder() { return m_isOccluder; }

	// type
	virtual int getType(void) { return M_OBJECT3D_ENTITY; }
	virtual void setActive(bool active);

	/**
	 * @brief Changes if the OEntity is invisible or not.
	 *
	 * This value is not used for culling.
	 * @param invisible A boolean value.
	 * @see setVisible
	 */
	void setInvisible(bool invisible) { m_isInvisible = invisible; }

	/**
	 * @brief Checks if the OEntity is invisible.
	 * @return A boolean value.
	 */
	bool isInvisible(void) { return m_isInvisible; }

	/**
	 * @brief Returns the Mesh of the OEntity.
	 * @return The Mesh object.
	 */
	Mesh *getMesh(void);
	
	/**
	 * @brief Sets the MeshRef of the OEntity.
	 * @param meshRef The MeshRef.
	 * @see MeshRef
	 */
	void setMeshRef(MeshRef *meshRef);
	
	/**
	 * @brief Returns the MeshRef of the OEntity.
	 * @return The MeshRef.
	 * @see MeshRef
	 */
	inline MeshRef *getMeshRef(void) { return m_meshRef; }

	/**
	 * @brief Changes the currently running animation to the one with the 
	 * given ID.
	 * @param animationId The animation ID.
	 */
	void changeAnimation(unsigned int animationId);
	
	/**
	 * @brief Checks if the currently running animation has finished.
	 * @return A boolean value.
	 */
	bool isAnimationOver(void);

	/**
	 * @brief Changes the animation speed.
	 * @param animationSpeed The new animation speed.
	 */
	inline void setAnimationSpeed(float animationSpeed)
	{
		m_animationSpeed = animationSpeed;
	}

	/**
	 * @brief Changes the current frame in the current animation.
	 * @param currentFrame The new frame to display.
	 */
	inline void setCurrentFrame(float currentFrame)
	{
		m_currentFrame = currentFrame;
	}

	/**
	 * @brief Returns the animation ID of the currently running animation.
	 * @return The animation ID.
	 */
	inline unsigned int getAnimationId(void) { return m_animationId; }

	/**
	 * @brief Returns the animation speed of the currently running animation.
	 * @return The animation speed.
	 */	
	inline float getAnimationSpeed(void) { return m_animationSpeed; }
	
	/**
	 * @brief Returns the animation current frame in the currently 
	 * running animation.
	 * @return The current frame.
	 */
	inline float getCurrentFrame(void) { return m_currentFrame; }

	/**
	 * @brief Disables physics and deletes the physics properties.
	 */
	void deletePhysicsProperties(void);
	
	/**
	 * @brief Enables physics and creates a new PhysicsProperties object.
	 * @return The new PhysicsProperties object.
	 */
	PhysicsProperties *createPhysicsProperties(void);

	/**
	 * @brief Returns the physics properties object.
	 * @return The PhysicsProperties.
	 */ 
	inline PhysicsProperties *getPhysicsProperties(void)
	{
		return m_physicsProperties;
	}

	/**
	 * @brief Returns the bounding box.
	 * @return The bounding box.
	 * @see Box3d
	 */
	inline Box3d *getBoundingBox(void) { return &m_boundingBox; }

	/**
	 * @brief Checks if the OEntity has a shadow.
	 * @return A boolean value.
	 */
	inline bool hasShadow() { return m_hasShadow; }
	
	/**
	 * @brief Enables or disables shadows for the OEntity.
	 * @param shadow The enable value.
	 */
	inline void enableShadow(bool shadow) { m_hasShadow = shadow; }

	virtual void update(void);
	virtual void updateVisibility(OCamera *camera);
};
}
#endif

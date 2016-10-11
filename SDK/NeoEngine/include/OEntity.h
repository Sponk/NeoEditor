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

/**
 * @brief Defines various types of collision shapes used for collision detection.
 */
enum COLLISION_SHAPE_TYPE
{
	/// Uses the bounding box for collision detection
	COLLISION_SHAPE_BOX = 0,
	/// Uses the bounding sphere for collision detection
	COLLISION_SHAPE_SPHERE,
	/// Uses a cone that completely surrounds the object for collision detection
	COLLISION_SHAPE_CONE,
	/// Generate a capsule around the object and uses that for collision detection
	COLLISION_SHAPE_CAPSULE,
	/// Uses a cylinder that surrounds the object for collision detection
	COLLISION_SHAPE_CYLINDER,
	/// Generates a convex hull around the object for collision detection
	COLLISION_SHAPE_CONVEX_HULL,
	/// Uses the triangle mesh directly for collision detection
	COLLISION_SHAPE_TRIANGLE_MESH
};

/**
 * @fixme Move all public interface into privates!
 */
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

	const char* getParentName()
	{
		return parentName.getSafeString();
	}

	void setParentName(const char* parentName)
	{
		PhysicsConstraint::parentName = parentName;
	}

	const Vector3& getPivot() const
	{
		return pivot;
	}

	void setPivot(const Vector3& pivot)
	{
		PhysicsConstraint::pivot = pivot;
	}

	const Vector3& getLowerLinearLimit() const
	{
		return lowerLinearLimit;
	}

	void setLowerLinearLimit(const Vector3& lowerLinearLimit)
	{
		PhysicsConstraint::lowerLinearLimit = lowerLinearLimit;
	}

	const Vector3& getUpperLinearLimit() const
	{
		return upperLinearLimit;
	}

	void setUpperLinearLimit(const Vector3& upperLinearLimit)
	{
		PhysicsConstraint::upperLinearLimit = upperLinearLimit;
	}

	const Vector3& getLowerAngularLimit() const
	{
		return lowerAngularLimit;
	}

	void setLowerAngularLimit(const Vector3& lowerAngularLimit)
	{
		PhysicsConstraint::lowerAngularLimit = lowerAngularLimit;
	}

	const Vector3& getUpperAngularLimit() const
	{
		return upperAngularLimit;
	}

	void setUpperAngularLimit(const Vector3& upperAngularLimit)
	{
		PhysicsConstraint::upperAngularLimit = upperAngularLimit;
	}

	bool isDisableParentCollision() const
	{
		return disableParentCollision;
	}

	void setDisableParentCollision(bool disableParentCollision)
	{
		PhysicsConstraint::disableParentCollision = disableParentCollision;
	}
};

/**
 * @brief The PhysicsProperties class contains all physical properties an object can possibly possess.
 */
class NEO_ENGINE_EXPORT PhysicsProperties
{
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

	PhysicsProperties(void);
	PhysicsProperties(const PhysicsProperties & physicsProperties);
	~PhysicsProperties(void);

	/**
	 * @brief Deletes the physics constraint from memory.
	 */
	void deleteConstraint(void);
	PhysicsConstraint * createConstraint(void);
	
	/**
	 * @brief Retrieves a pointer to the physics constraint object.
	 * @return The PhysicsConstraint object.
	 */
	inline PhysicsConstraint * getConstraint(void){ return m_constraint; }

	/**
	 * @brief Changes the physics shape ID that is given out by the physics context.
	 * @param shapeId The new shape ID.
	 */
	inline void setShapeId(unsigned int shapeId){ m_shapeId = shapeId; }
	
	/**
	 * @brief Retrieves the current physics ID.
	 * @return The shape ID.
	 */
	inline unsigned int getShapeId(void){ return m_shapeId; }

	/**
	 * @brief Changes the collision object ID used by the physics context.
	 * @param collisionObjectId The new collision object ID.
	 */
	inline void setCollisionObjectId(unsigned int collisionObjectId){ m_collisionObjectId = collisionObjectId; }
	
	/**
	 * @brief Retrieves the current collision object ID that is used by the physics context.
	 * @return the collision object ID.
	 */
	inline unsigned int getCollisionObjectId(void){ return m_collisionObjectId; }

	/**
	 * @brief Changes the collision shape used for collision detection.
	 * @param collisionShape The new shape.
	 */
	inline void setCollisionShape(COLLISION_SHAPE_TYPE collisionShape){ m_collisionShape = collisionShape; }
	
	/**
	 * @brief Retrieves the current collision shape that is used for collision detection.
	 * @return the collision shape.
	 */
	inline COLLISION_SHAPE_TYPE getCollisionShape(void){ return m_collisionShape; }

	/**
	 * @brief Sets the ghost property to the given value.
	 * @param ghost The new value.
	 * @see isGhost
	 */
	inline void setGhost(bool ghost){ m_ghost = ghost; }
	
	/**
	 * @brief Checks if the given object is a physical "ghost".
	 * 
	 * A ghost is an object that has physical properties in a scene but
	 * does not actively react to collisions. This property is useful
	 * for implementing triggers and items that detect collision but
	 * do not react with the player.
	 * 
	 * @return A boolean value.
	 */
	inline bool isGhost(void){ return m_ghost; }

	/**
	 * @brief Changes the mass.
	 * 
	 * The mass of an object is needed for it to react to physical collisions.
	 * If the mass is set to 0 the object will be static and will not
	 * react to any collision or force applied to it.
	 * 
	 * @param mass The new mass.
	 */
	inline void setMass(float mass){ m_mass = mass; }
	
	/**
	 * @brief Retrieves the mass of the object.
	 * @return The mass.
	 * @see setMass
	 */
	inline float getMass(void){ return m_mass; }

	/**
	 * @brief Changes the friction coefficient used for calculations.
	 * @param friction The new value.
	 */
	inline void setFriction(float friction){ m_friction = friction; }
	
	/**
	 * @brief Retrieves the current friction value.
	 * @return The current value.
	 */
	inline float getFriction(void){ return m_friction; }

	/**
	 * @brief Changes the restitution coefficient used for calculations.
	 * @param restitution The new value.
	 */
	inline void setRestitution(float restitution){ m_restitution = restitution; }
	
	/**
	 * @brief Retrieves the current restitution value.
	 * @return The current value.
	 */
	inline float getRestitution(void){ return m_restitution; }

	/**
	 * @brief Changes the linear damping coefficient used for calculations.
	 * @param linearDamping The new value.
	 */
	inline void setLinearDamping(float linearDamping){ m_linearDamping = linearDamping; }
	
	/**
	 * @brief Retrieves the current linear damping value.
	 * @return The current value.
	 */
	inline float getLinearDamping(void){ return m_linearDamping; }

	/**
	 * @brief Changes the angular damping coefficient used for calculations.
	 * @param angularDamping The new value.
	 */
	inline void setAngularDamping(float angularDamping){ m_angularDamping = angularDamping; }
	
	/**
	 * @brief Retrieves the current angular damping value.
	 * @return The current value.
	 */
	inline float getAngularDamping(void){ return m_angularDamping; }

	/**
	 * @brief Changes the angular factor coefficient used for calculations.
	 * @param angularFactor The new value.
	 */
	inline void setAngularFactor(float angularFactor){ m_angularFactor = angularFactor; }
	
	/**
	 * @brief Retrieves the current angular factor value.
	 * @return The current value.
	 */
	inline float getAngularFactor(void){ return m_angularFactor; }

	/**
	 * @brief Changes the linear factor coefficient used for calculations.
	 * @param linearFactor The new value.
	 */
	inline void setLinearFactor(const Vector3 & linearFactor){ m_linearFactor = linearFactor; }
	
	/**
	 * @brief Retrieves the current linear factor value.
	 * @return The current value.
	 */
	inline Vector3 * getLinearFactor(void){ return &m_linearFactor; }
	
	/**
	 * @brief Notifies the physics engine of all values that are set here.
	 * 
	 * It does not update the collision shape type, it only sets all values properly.
	 * This method requires the physics object to be already created.
	 */
	void updateProperties();
};

struct NEO_ENGINE_EXPORT RayCastResult
{
	bool hit;
	Vector3 point;
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
	bool m_wireframe;
	
	Material* m_materials;
	unsigned int m_numMaterials;

    Texture** m_textures;
    unsigned int m_numTextures;

public:

	// constructor / destructor
	OEntity(MeshRef *meshRef);
	~OEntity(void);

	// copy constructor
	OEntity(const OEntity &entity);

	RayCastResult castRay(const Vector3& rayO, const Vector3& rayD);

	Texture* getTexture(size_t idx = 0) const { return m_textures[idx]; }
	size_t getTexturesNumber() const { return m_numTextures; }

	/**
	 * @brief Retrieves the material with the given index.
	 * @param idx The index to retrieve from.
	 */
	Material* getMaterial(size_t idx = 0) { return &m_materials[idx]; }
	
	/**
	 * @brief Returns the number of materials attached to this entity.
	 * @return The number of materials.
	 */
	size_t getMaterialsNumber() { return m_numMaterials; }

	/**
	 * @brief Checks if the entity has the wireframe mode enabled.
	 * @return A boolean value.
	 */
	bool hasWireframe() { return m_wireframe; }
	
	/**
	 * @brief Enables/Disable the wireframe of the entity.
	 * @param v A boolean value.
	 */
	void enableWireframe(bool v) { m_wireframe = v; }

	/**
	 * @brief Checks if the entity has any transparent material.
	 * @return A boolean value.
	 */
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

	virtual int getType(void) { return OBJECT3D_ENTITY; }
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

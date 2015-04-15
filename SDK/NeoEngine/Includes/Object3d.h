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

#ifndef __OBJECT3D_H
#define __OBJECT3D_H

namespace Neo
{

/**
 * @brief The Object3d class represents a general object in a 3D scene.
 *
 * You can add new types of objects by creating sub-classes that extend this
 * class.
 *
 * @see OEntity
 * @see OSound
 * @see OText
 * @see OLight
 */
class NEO_ENGINE_EXPORT Object3d
{
public:

	// constructor
	Object3d(void);

	// destructor
	virtual ~Object3d(void);

	// copy constructor
	Object3d(const Object3d & object);

protected:

	/// The name of the object
	String m_name;

	/// The positopm
	Vector3 m_position;

	/// The scale
	Vector3 m_scale;

	/// The rotation
	Quaternion m_rotation;

	/// The matrix
	Matrix4x4 m_matrix;

	/// List of children
	vector<Object3d*> m_children;

    // Attributes
	#if __cplusplus >= 201103L
		typedef unordered_map<string, NeoVariable>::iterator AttributeIterator ;
		unordered_map <string, NeoVariable> m_attributes;
	#else
		typedef map<string, NeoVariable>::iterator AttributeIterator;
		map <string, NeoVariable> m_attributes;
	#endif

	// properties
	bool m_isActive;
	bool m_isVisible;
	bool m_needToUpdate;

	unsigned long m_id;

	// linking
	Object3d * m_parent;

	/// List of behaviors
	vector <Behavior *> m_behaviors;

private:
	/**
	 * @brief Removes the given child from the children list.
	 */
	void removeChild(Object3d* child);

protected:

	void clearObject3d(void);

public:

	/**
	 * @brief Recalculates the internal matrix relative to the parent object.
	 */
	void updateMatrix(void);

	/**
	 * @brief Calculates the local matrix without respecting the parent.
	 */
	void computeLocalMatrix(void);

	/**
	 * @brief Returns the current matrix.
	 * @return The pointer to the internal matrix.
	 * @see Matrix4x4
	 */
	inline Matrix4x4 *	getMatrix(void){ return &m_matrix; }

	/**
	 * @brief Unlinks all children from this object and sets their parent to NULL.
	 */
	void unlinkChildren(void);

	/**
	 * @brief Calculates the matrix of every child object.
	 */
	void computeChildrenMatrices(void);

    /**
	 * @brief Sets the value of the attribute with the given name.
	 * @param name The name of the variable to set.
	 * @param variable The variable containing the data.
	 */
	inline void setAttribute(const char* name, NeoVariable variable)
	{
		m_attributes[name] = variable;
	}

	/**
	 * @brief Returns the value of the attribute variable with the given name.
	 * @param name The name of the variable to fetch.
	 * @return The variable containing the data.
	 */
	inline NeoVariable getAttribute(const char* name)
	{
		return m_attributes[name];
	};

	/**
	 * @brief Rotates the given vector while preserving its length
	 * and returns the result.
	 *
	 * @param vector The vector to rotate.
	 * @return The uniform rotated vector.
	 */
	Vector3 getUniformRotatedVector(const Vector3 & vector);

	/**
	 * @brief Calculates the inverse rotated vector.
	 * @param vector The vector to rotate.
	 * @return The rotated vector.
	 */
	inline Vector3 getInverseRotatedVector(const Vector3& vector) const
	{
		return m_matrix.getInverseRotatedVector3(vector);
	}

	/**
	 * @brief Rotates the given vector and returns the result.
	 * @param vector The vector to rotate.
	 * @return The rotated vector.
	 */
	inline Vector3 getRotatedVector(const Vector3& vector) const
	{
		return m_matrix.getRotatedVector3(vector);
	}

	/**
	 * @brief Multiplies the inverse matrix with the given vector and return the
	 * result.
	 *
	 * Pi = inv(M) * P
	 *
	 * @param position The vector to multiply.
	 * @return The result.
	 */
	inline Vector3 getInversePosition(const Vector3& position) const
	{
		return m_matrix.getInverse() * position;
	}

	/**
	 * @brief Transforms the given vector using the internal matrix.
	 *
	 * Vt = M * V
	 *
	 * @param The vector to transform.
	 * @return The result.
	 */
	inline Vector3 getTransformedVector(const Vector3& vector) const
	{
		return m_matrix * vector;
	}

	/**
	 * @brief Changes the position.
	 * @param position The new position.
	 */
	void setPosition(const Vector3& position);

	/**
	 * @brief Returns the transformed position directly from the internal matrix.
	 * @return The transformed position.
	 */
	inline Vector3 getTransformedPosition(void) const
	{
		return Vector3(m_matrix.entries[12], m_matrix.entries[13],
					   m_matrix.entries[14]);
	}

	/**
	 * @brief Returns the position as a Vector3.
	 * @return The current position.
	 */
	inline Vector3 getPosition(void) const { return m_position; }

	/**
	 * @brief Changes the Euler rotation.
	 *
	 * The unit is degrees.
	 * @param euler The vector containing the new value in deggrees for each axis.
	 */
	void setEulerRotation(const Vector3& euler);

	/**
	 * @brief Changes the angle of the specified axis.
	 *
	 * @code
	 * object->setAxisAngleRotation(Vector3(1.0f,0.0f,0.0f), 15.0f);
	 * @endcode
	 *
	 * @param axis The axis to change. Only one axis should be 1.0f at a time!
	 * @param angle The angle in degrees.
	 */
	void setAxisAngleRotation(const Vector3& axis, float angle);

	/**
	 * @brief Changes the angle of the specified axis by adding the given value.
	 *
	 * @code
	 * object->addAxisAngleRotation(Vector3(1.0f,0.0f,0.0f), 15.0f);
	 * @endcode
	 *
	 * @param axis The axis to change. Only one axis should be 1.0f at a time!
	 * @param angle The angle in degrees.
	 */
	void addAxisAngleRotation(const Vector3& axis, float angle);

	/**
	 * @brief Sets the rotation or the object.
	 * @param rotation The new rotation.
	 * @see Quaternion
	 */
	void setRotation(const Quaternion& rotation);

	/**
	 * @brief Returns the transformed rotation.
	 * @return The Transformed rotation.
	 */
	inline Vector3 getTransformedRotation(void) const
	{
		return m_matrix.getEulerAngles();
	}

	/**
	 * @brief Returns the Euler rotation in a Vector3 in degrees.
	 * @return The Euler rotation.
	 */
	inline Vector3 getEulerRotation(void) const
	{
		return m_rotation.getEulerAngles();
	}

	/**
	 * @brief Returns the rotation of the object.
	 * @return The rotation.
	 * @see Quaternion
	 */
	inline Quaternion getRotation(void) const { return m_rotation; }

	/**
	 * @brief Changes the scale of the object.
	 * @param scale The new scale.
	 */
	void setScale(const Vector3& scale);

	/**
	 * @brief Returns the transformed scale of the object.
	 * @return The transformed scale.
	 */
	inline Vector3 getTransformedScale(void) const
	{
		return m_matrix.getScale();
	}

	/**
	 * @brief Returns the scale of the object.
	 * @return The scale of the object.
	 */
	inline Vector3 getScale(void) const { return m_scale; }

	/**
	 * @brief Links the object to another object.
	 *
	 * Linking means recalculating positon, rotation and scale of the object
	 * to become relative to the given parent object.
	 *
	 * @param parent The parent object.
	 * @see unLink
	 */
	void linkTo(Object3d* parent);

	/**
	 * @brief Removing the link to the parent object.
	 * @see linkTo
	 */
	void unLink(void);

	/**
	 * @brief Sets the parent object.
	 *
	 * Attention: Does not link to the parent!
	 * @param object The new parent.
	 * @see linkTo
	 */
	inline void setParent(Object3d* object) { m_parent = object; }

	/**
	 * @brief Adds an Object3d to the list of children.
	 * @param child The new child object.
	 */
	inline void addChild(Object3d* child) { m_children.push_back(child); }

	/**
	 * @brief Checks if the object has a parent.
	 * @return A boolean value.
	 */
	inline bool hasParent(void) { return (m_parent != NULL); }

	/**
	 * @brief Returns the number of children.
	 * @return The number of children.
	 */
	inline unsigned int getChildrenNumber(void) { return m_children.size(); }

	/**
	 * @brief Returns the parent object.
	 * @return The parent object.
	 */
	inline Object3d* getParent(void) { return m_parent; }

	/**
	 * @brief Returns the child with the given ID.
	 *
	 * Attention: The ID might change if children are deleted!
	 * @param id The ID of the child.
	 */
	inline Object3d* getChild(unsigned int id) { return m_children[id]; }

	/**
	 * @brief Returns the ID of the object.
	 * @return The ID.
	 */
	unsigned long getId() { return m_id; }

	/**
	 * @brief Changes the ID of the object.
	 * @param id The new ID.
	 */
	void setId(unsigned long id) { m_id = id; }

	/**
	 * @brief Updates all behaviors.
	 */
	void updateBehaviors(void);

	/**
	 * @brief Draws all behaviors.
	 */
	void drawBehaviors(void);

	/**
	 * @brief Deletes the Behavior with the given ID.
	 * @param id The ID to delete.
	 */
	void deleteBehavior(unsigned int id);

	/**
	 * @brief Switches the IDs of the given behavior IDs.
	 * @param idA The ID of the first object.
	 * @param idB The ID of the second object.
	 */
	void invertBehavior(unsigned int idA, unsigned int idB);

	/**
	 * @brief Replaces the behavior with the given ID with the given Behavior.
	 * @param id The ID of the behavior to replace.
	 * @param behavior The replacement.
	 */
	void changeBehavior(unsigned int id, Behavior* behavior);

	/**
	 * @brief Adds a new behavior to the list of behaviors.
	 *
	 * The ID can be retrieved by calling getBehaviorsNumber.
	 * @param behavior The new behavior.
	 * @see getBehaviorsNumber
	 */
	inline void addBehavior(Behavior* behavior)
	{
		m_behaviors.push_back(behavior);
	}

	/**
	 * @brief Returns the number of registered Behaviors.
	 * @return The number of registered Behaviors.
	 */
	inline unsigned int getBehaviorsNumber(void) { return m_behaviors.size(); }

	/**
	 * @brief Returns the Behavior with the given ID.
	 * @param id The ID of the Behavior.
	 * @return The Behavior.
	 */
	inline Behavior* getBehavior(unsigned int id) { return m_behaviors[id]; }

	/**
	 * @brief Checks if the object needs to be updated.
	 * @see update
	 */
	inline bool needToUpdate(void) { return m_needToUpdate; }

	/**
	 * @brief Changes if the object is being active or inactive.
	 * @param active The new activity value.
	 */
	virtual void setActive(bool active) { m_isActive = active; }

	/**
	 * @brief Checks if the object is active.
	 * @return A boolean value.
	 */
	inline bool isActive(void) { return m_isActive; }

	/**
	 * @brief Changes if the object is visible or not.
	 *
	 * This value is used for culling and will be updated on every frame.
	 * This makes setting it manually useless. For OEntity objects you can use
	 * OEntity::setInvisible to overwrite the visibility.
	 *
	 * @param visible The new visibility value.
	 */
	inline void setVisible(bool visible) { m_isVisible = visible; }

	/**
	 * @brief Checks if the object is visible.
	 * @return A boolean value.
	 * @see setVisible
	 */
	inline bool isVisible(void) { return m_isVisible; }

	/**
	 * @brief Recalculates visibility for the given camera.
	 *
	 * Used for culling.
	 * @param camera The camera to check visibility for.
	 */
	virtual void updateVisibility(OCamera* camera) {}

	/**
	 * @brief Change the object name.
	 *
	 * Attention: Names must be unique!
	 * @param name The new name.
	 */
	void setName(const char* name);

	/**
	 * @brief Returns the object name.
	 * @return The name of the object.
	 */
	inline const char* getName(void) { return m_name.getSafeString(); }

	/**
	 * @brief Returns the type of the object.
	 * @return The type.
	 */
	virtual int getType(void) { return M_OBJECT3D; }

	/**
	 * @brief Updates the object.
	 */
	virtual void update(void);
};
}

#endif

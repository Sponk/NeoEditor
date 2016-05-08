#ifndef __OBJECT2D_H
#define __OBJECT2D_H

#include <Neo2D.h>
#include <NeoCore.h>
#include <memory>

namespace Neo2D
{

/**
 * @brief An Object2D represents any type of object in 2D space.
 *
 * It provides several mechanisms common among all 2D objects like translation, rotation and similar as well as
 * a parent/child relationship model.
 */
class NEO2D_EXPORT Object2D
{
	Neo::Vector2 m_position;
	float m_rotation;
	Neo::Vector2 m_size;
	weak_ptr<Object2D> m_parent;
	bool m_invisible;
	bool m_active;

public:
	/**
	 * @brief Constructs a new Object2D instance by copying another one.
	 * @param o The object to copy.
	 */
	Object2D(const Object2D& o) :
		m_position(o.m_position),
		m_rotation(o.m_rotation),
		m_size(o.m_size),
		m_parent(o.m_parent),
		m_invisible(o.m_invisible),
		m_active(o.m_active)
	{}

	/**
	 * @brief Constructs a new Object2D instance.
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @param w The width.
	 * @param h The height.
	 * @param parent The parent object.
	 */
	Object2D(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent) :
		m_position(x, y),
		m_rotation(0),
		m_size(w, h),
		m_parent(parent),
		m_invisible(false),
		m_active(true)
	{}

	/**
	 * @brief Retrieves the current 2D position of the object.
	 * @return The position in 2D space.
	 */
	const Neo::Vector2& getPosition() const
	{
		return m_position;
	}

	/**
	 * @brief Changes the 2D position of the object.
	 * @param m_position The new position.
	 */
	void setPosition(const Neo::Vector2 &m_position)
	{
		Object2D::m_position = m_position;
	}

	/**
	 * @brief Retrieves the rotation of the object as an Euler angle.
	 * @return The rotation.
	 */
	float getRotation() const
	{
		return m_rotation;
	}

	/**
	 * @brief Changes the rotation of the object.
	 * @param m_rotation The new rotation as an Euler angle.
	 */
	void setRotation(float m_rotation)
	{
		Object2D::m_rotation = m_rotation;
	}

	/**
	 * @brief Retrieves the size of the object.
	 * @return The 2D vector containing the size.
	 */
	const Neo::Vector2 &getSize() const
	{
		return m_size;
	}

	/**
	 * @brief Changes the size of the object.
	 * @param m_size The new size as a 2D vector.
	 */
	void setSize(const Neo::Vector2 &m_size)
	{
		Object2D::m_size = m_size;
	}

	/**
	 * @brief Retrieves the parent of the object.
	 * @return A weak pointer to the parent.
	 */
	const weak_ptr<Object2D>& getParent() const
	{
		return m_parent;
	}

	/**
	 * @brief Changes the parent of the object.
	 * @param m_parent A weak pointer to the new parent.
	 */
	void setParent(const weak_ptr<Object2D>& m_parent)
	{
		Object2D::m_parent = m_parent;
	}

	/**
	 * @brief Checks if the object is invisible.
	 * @return A boolean value.
	 */
	bool isInvisible() const
	{
		return m_invisible;
	}

	/**
	 * @brief Changes the visibility of the object.
	 * @param m_invisible The new invisible value.
	 */
	void setInvisible(bool m_invisible)
	{
		Object2D::m_invisible = m_invisible;
	}

	/**
	 * @brief Checks if the object is currently active.
	 * @return A boolean value.
	 */
	bool isActive() const
	{
		return m_active;
	}

	/**
	 * @brief Changes the activity state of the object.
	 * @param m_active The new boolean state.
	 */
	void setActive(bool m_active)
	{
		Object2D::m_active = m_active;
	}

	/**
	 * @brief Checks if a given point is inside the objects bounds.
	 * @param point The point to check.
	 * @return A boolean value.
	 */
	bool contains(const Neo::Vector2& point)
	{
		return (point.x >= m_position.x && point.x <= m_position.x + m_size.x)
			&& (point.y >= m_position.y && point.y <= m_position.y + m_size.y);
	}

	/**
	 * @brief Draws the object with an offset of (0,0).
	 */
	void draw() { draw(Neo::Vector2()); }

	/**
	 * @brief Updates the object with a frame delta of 0.0 seconds.
	 */
	void update() { update(0.0); }

	/**
	 * @brief Draws the object with a given 2D position offset.
	 * @param offset The position offset.
	 */
	virtual void draw(const Neo::Vector2& offset) {}

	/**
	 * @brief Update the object.
	 * @param dt The frame delta time.
	 */
	virtual void update(float dt) {}
};
}

#endif

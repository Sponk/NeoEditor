#ifndef __OBJECT2D_H
#define __OBJECT2D_H

#include <Neo2D.h>
#include <NeoCore.h>
#include <memory>

namespace Neo2D
{
class NEO2D_EXPORT Object2D
{
	Neo::Vector2 m_position;
	float m_rotation;
	Neo::Vector2 m_size;
	weak_ptr<Object2D> m_parent;
	bool m_invisible;
	bool m_active;

private:
public:
	Object2D(const Object2D& o) :
		m_position(o.m_position),
		m_rotation(o.m_rotation),
		m_size(o.m_size),
		m_parent(o.m_parent),
		m_invisible(o.m_invisible),
		m_active(o.m_active)
	{}

	Object2D(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent) :
		m_position(x, y),
		m_rotation(0),
		m_size(w, h),
		m_parent(parent),
		m_invisible(false),
		m_active(true)
	{}

	const Neo::Vector2& getPosition() const
	{
		return m_position;
	}

	void setPosition(const Neo::Vector2 &m_position)
	{
		Object2D::m_position = m_position;
	}

	float getRotation() const
	{
		return m_rotation;
	}

	void setRotation(float m_rotation)
	{
		Object2D::m_rotation = m_rotation;
	}

	const Neo::Vector2 &getSize() const
	{
		return m_size;
	}

	void setSize(const Neo::Vector2 &m_size)
	{
		Object2D::m_size = m_size;
	}

	const weak_ptr<Object2D>& getParent() const
	{
		return m_parent;
	}

	void setParent(const weak_ptr<Object2D>& m_parent)
	{
		Object2D::m_parent = m_parent;
	}

	bool isInvisible() const
	{
		return m_invisible;
	}

	void setInvisible(bool m_invisible)
	{
		Object2D::m_invisible = m_invisible;
	}

	bool isActive() const
	{
		return m_active;
	}

	void setActive(bool m_active)
	{
		Object2D::m_active = m_active;
	}

	bool contains(const Neo::Vector2& point)
	{
		return (point.x >= m_position.x && point.x <= m_position.x + m_size.x)
			&& (point.y >= m_position.y && point.y <= m_position.y + m_size.y);
	}

	void draw() { draw(Neo::Vector2()); }
	void update() { update(0.0); }

	virtual void draw(const Neo::Vector2& offset) {}
	virtual void update(float dt) {}
};
}

#endif

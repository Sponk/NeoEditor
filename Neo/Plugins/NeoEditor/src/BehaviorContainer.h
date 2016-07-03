#ifndef __BEHAVIOR_CONTAINER_H
#define __BEHAVIOR_CONTAINER_H

#include <Container.h>
#include <NeoEngine.h>

class BehaviorContainer : public Neo2D::Gui::Widget
{
	Neo::Object3d* m_currentObject;
	size_t m_numBehaviors; /// Contains the number of behaviors to detect changes
	shared_ptr<Neo2D::Gui::Container> m_container;
	
public:
	BehaviorContainer(int x, int y, unsigned int w, unsigned int h,
					  const shared_ptr<Object2D>& parent)
		: Widget(x, y, w, h, "", parent),
		  m_container(make_shared<Neo2D::Gui::Container>(0,0,0,0, nullptr))
	{}

	void displayObject(Neo::Object3d* o);
	void setLayout(const shared_ptr<Neo2D::Gui::LayoutStrategy>& l) { m_container->setLayout(l); }
	void activateChildren(bool b) { setActive(b); m_container->activateChildren(b); }

	void draw(const Neo::Vector2& offset)
	{
		m_container->draw(offset);
	}

	void update(float dt)
	{
		m_container->setInvisible(isInvisible());
		m_container->setPosition(getPosition());
		setSize(m_container->calculateContentSize());
		m_container->setSize(getSize());
		m_container->update(dt);
	}
};


#endif

#ifndef __BEHAVIOR_CONTAINER_H
#define __BEHAVIOR_CONTAINER_H

#include <Container.h>
#include <NeoEngine.h>

class BehaviorContainer : public Neo2D::Gui::Container
{
	Neo::Object3d* m_currentObject;
	size_t m_numBehaviors; /// Contains the number of behaviors to detect changes
	
public:
	BehaviorContainer(int x, int y, unsigned int w, unsigned int h,
					  const shared_ptr<Object2D>& parent)
		: Container(x, y, w, h, parent)
	{}

	void displayObject(Neo::Object3d* o);
};


#endif

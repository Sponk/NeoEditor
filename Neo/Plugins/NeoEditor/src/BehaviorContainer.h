#ifndef __BEHAVIOR_CONTAINER_H
#define __BEHAVIOR_CONTAINER_H

#include <Container.h>
#include <NeoEngine.h>

class BehaviorContainer : public Neo2D::Gui::Container
{
	Neo::Object3d* m_currentObject;
	size_t m_numBehaviors; /// Contains the number of behaviors to detect changes

	bool m_requestUpdate; /// A flag that is set when the next call to update should also rebuild the UI.
public:
	BehaviorContainer(int x, int y, unsigned int w, unsigned int h,
					  const shared_ptr<Object2D>& parent)
		: Container(x, y, w, h, parent),
		  m_requestUpdate(false)
	{}

	void displayObject(Neo::Object3d* o);
	void update(float dt) override;
};


#endif

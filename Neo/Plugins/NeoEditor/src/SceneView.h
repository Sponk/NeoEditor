#ifndef NEO_SCENEVIEW_H
#define NEO_SCENEVIEW_H

#include <Widget.h>

class SceneView : public Neo2D::Gui::Widget
{
	Neo::OCamera m_camera;

public:
	SceneView(int x,
			  int y,
			  unsigned int w,
			  unsigned int h,
			  const shared_ptr<Neo2D::Object2D>& parent);

	virtual void handle(const Neo2D::Gui::Event& e);
	virtual void draw(const Neo::Vector2& offset);
	virtual void update(float dt);
	void resetCamera();
};

#endif //NEO_SCENEVIEW_H

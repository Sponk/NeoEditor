#ifndef NEO_SCENEVIEW_H
#define NEO_SCENEVIEW_H

#include <Widget.h>
#include <vector>

class SceneView : public Neo2D::Gui::Widget
{
	Neo::OCamera m_camera;
	std::vector<Neo::Object3d*> m_selection;

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

	std::vector<Neo::Object3d*>& getSelection() { return m_selection; }
	void clearSelection();
	void addSelectedObject(Neo::Object3d* object);
};

#endif //NEO_SCENEVIEW_H

#ifndef NEO_SCENEVIEW_H
#define NEO_SCENEVIEW_H

#include <Widget.h>
#include <vector>

class SceneView : public Neo2D::Gui::Widget
{
	shared_ptr<Neo::Scene> m_handlesScene;
	shared_ptr<Neo::Scene> m_overlayScene;
	Neo::OCamera m_camera;
	std::vector<Neo::Object3d*> m_selection;
	
	struct Handles
	{
		Neo::OEntity* x, *y, *z;
		Neo::OEntity* grabbed;

		void enable(bool v)
		{
			x->setInvisible(!v);
			y->setInvisible(!v);
			z->setInvisible(!v);

			x->setActive(v);
			y->setActive(v);
			z->setActive(v);		
		}

		void setPosition(Neo::Vector3 p)
		{
			x->setPosition(p);
			y->setPosition(p);
			z->setPosition(p);

			x->updateMatrix();
			y->updateMatrix();
			z->updateMatrix();
		}
		
		void setScale(Neo::Vector3 p)
		{
			x->setScale(p);
			y->setScale(p);
			z->setScale(p);

			x->updateMatrix();
			y->updateMatrix();
			z->updateMatrix();
		}
	};

	Handles& m_currentHandles;
	Handles m_rotation, m_translation, m_scale;	
	
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
	Neo::Vector3 getSelectionCenter();
	
	void updateOverlayScene();
};

#endif //NEO_SCENEVIEW_H

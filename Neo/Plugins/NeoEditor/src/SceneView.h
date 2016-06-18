#ifndef NEO_SCENEVIEW_H
#define NEO_SCENEVIEW_H

#include <Widget.h>
#include <vector>
#include "UndoQueue.h"

enum HANDLE_MODE
{
	TRANSLATION,
	SCALE,
	ROTATION
};

class SceneView : public Neo2D::Gui::Widget
{
	Neo::Level m_level;
	Neo::Scene* m_handlesScene;
	Neo::Scene* m_overlayScene;
	Neo::OCamera m_camera;
	std::vector<Neo::Object3d*> m_selection;

	/// Transform objects either relative to their own transformation or relative to the selection.
	bool m_objectLocalTransformation;
		
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

	Handles* m_currentHandles;
	Handles m_rotation, m_translation, m_scale;	
	HANDLE_MODE m_mode;
	UndoQueue& m_undo;

	void rotationHandle(Neo::OEntity* handleEntity, const Neo::Vector3& axis, const Neo::Vector3& mousepos);
	void scaleHandle(Neo::OEntity* handleEntity, const Neo::Vector3& axis, const Neo::Vector3& mousepos);
	void translationHandle(Neo::OEntity* handleEntity, const Neo::Vector3& axis, const Neo::Vector3& mousepos);

public:
	SceneView(UndoQueue& undo,
			  int x,
			  int y,
			  unsigned int w,
			  unsigned int h,
			  const shared_ptr<Neo2D::Object2D>& parent);

	virtual void init() override;
	virtual bool handle(const Neo2D::Gui::Event& e) override;
	virtual void draw(const Neo::Vector2& offset);
	virtual void update(float dt);
	void resetCamera();

	std::vector<Neo::Object3d*>& getSelection() { return m_selection; }
	void clearSelection();
	void addSelectedObject(Neo::Object3d* object);
	Neo::Vector3 getSelectionCenter();
	
	void clear() { m_level.clear(); m_overlayScene = nullptr; m_handlesScene = nullptr; }
	void clearOverlayScene();
	void updateOverlayScene();
	void setHandleMode(HANDLE_MODE mode);

	void setObjectLocal(bool v) { m_objectLocalTransformation = v; }
	bool isObjectLocal() const { return m_objectLocalTransformation; }
	
	using Neo2D::Gui::Widget::setState;
};

#endif //NEO_SCENEVIEW_H

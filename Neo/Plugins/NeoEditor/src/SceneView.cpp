#include "SceneView.h"
#include <CommonEvents.h>
#include <algorithm>

using namespace Neo;

SceneView::SceneView(int x,
					 int y,
					 unsigned int w,
					 unsigned int h,
					 const shared_ptr<Neo2D::Object2D>& parent)
	: Widget(x, y, w, h, nullptr, parent, nullptr)
{
	resetCamera();

	registerEvent(make_shared<Neo2D::Gui::MouseDeselectEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseLeftClickEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseRightClickEvent>(*this, nullptr, nullptr));

	updateOverlayScene();
}

void SceneView::resetCamera()
{
	m_camera.setPosition(Vector3(0,0,0));
	m_camera.setEulerRotation(Vector3(90,0,0));
	m_camera.setClippingFar(100000);
	m_camera.updateMatrix();
}

void SceneView::draw(const Neo::Vector2& offset)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	Vector2 size = engine->getSystemContext()->getScreenSize();
	render->setViewport(0, 0, size.x, size.y);
	render->setClearColor(m_camera.getClearColor());
	render->clear(BUFFER_COLOR | BUFFER_DEPTH);
	render->enableScissorTest();
	render->setScissor(getPosition().x, getPosition().y, getSize().x, getSize().y);

	m_camera.enable();
	m_camera.updateListener();

	auto scene = engine->getLevel()->getCurrentScene();

	scene->draw(&m_camera);
	scene->drawObjectsBehaviors();

	m_overlayScene->draw(&m_camera);

	render->disableScissorTest();
}

void SceneView::clearSelection()
{
	std::for_each(m_selection.begin(), m_selection.end(),
				  [](Object3d* object) {
					  if(object->getType() == OBJECT3D_ENTITY)
						  static_cast<OEntity*>(object)->enableWireframe(false);
				  });

	m_selection.clear();
}

void SceneView::addSelectedObject(Neo::Object3d* object)
{
	m_selection.push_back(object);

	if(object->getType() == OBJECT3D_ENTITY)
		static_cast<OEntity*>(object)->enableWireframe(true);
}

void SceneView::handle(const Neo2D::Gui::Event& e)
{
	switch(e.getType())
	{
		case Neo2D::Gui::MOUSE_RIGHT_CLICK:
			setState(Neo2D::Gui::WIDGET_SELECTED);
			break;

		case Neo2D::Gui::MOUSE_LEFT_CLICK:
		{
			setState(Neo2D::Gui::WIDGET_SELECTED);

			NeoEngine* engine = NeoEngine::getInstance();
			Scene* scene = engine->getLevel()->getCurrentScene();

			InputContext* input = engine->getInputContext();
			Vector2 screen = engine->getSystemContext()->getScreenSize();
			Vector2 mousepos = input->getMouse().getPosition();

			mousepos.y = screen.y - mousepos.y;
			Vector3 cameraAxis = m_camera.getRotatedVector(Vector3(0, 0, -1)).getNormalized();

			if(!input->isKeyDown(Neo::KEY_LSHIFT))
				clearSelection();

			Vector3 origin = m_camera.getTransformedPosition();
			Vector3 direction = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 1));

			if(m_camera.isOrtho())
			{
				Vector3 point;
				isRayPlaneIntersection(direction, -cameraAxis, origin + (cameraAxis * m_camera.getClippingNear()), cameraAxis, &point);
				origin = point;
			}

			direction = origin + ((direction - origin).getNormalized() * (m_camera.getClippingFar() - m_camera.getClippingNear()));

			Object3d* selected = nullptr;
			float selectedDistance = 0.0f;

			// Search in main scene
			for(size_t i = 0; i < scene->getEntitiesNumber(); i++)
			{
				auto result = scene->getEntityByIndex(i)->castRay(origin, direction);
				if(result.hit)
				{
					if(float newlength = (origin - result.hit).getLength() > selectedDistance
						|| selected == nullptr)
					{
						selected = scene->getEntityByIndex(i);
						selectedDistance = newlength;
					}
				}
			}

			// Search in overlay scene
			for(size_t i = 0; i < m_overlayScene->getEntitiesNumber(); i++)
			{
				auto result = m_overlayScene->getEntityByIndex(i)->castRay(origin, direction);
				if(result.hit)
				{
					if(float newlength = (origin - result.hit).getLength() > selectedDistance
						|| selected == nullptr)
					{
						selected = m_overlayScene->getEntityByIndex(i)->getParent();
						selectedDistance = newlength;
					}
				}
			}

			if(selected)
			{
				addSelectedObject(selected);
				doCallback();
			}
		}
			break;

		case Neo2D::Gui::MOUSE_DESELECT:
			setState(Neo2D::Gui::WIDGET_NORMAL);
			break;
	}
}

void SceneView::update(float dt)
{
	if(getState() != Neo2D::Gui::WIDGET_SELECTED)
		return;

	NeoEngine* engine = NeoEngine::getInstance();
	float speed = 60.0f * dt;
	auto input = engine->getInputContext();
	if (input->isKeyDown(KEY_W))
		m_camera.translate(Vector3(0, 0, -speed), true);
	else if (input->isKeyDown(KEY_S))
		m_camera.translate(Vector3(0, 0, speed), true);

	if (input->isKeyDown(KEY_A))
		m_camera.translate(Vector3(-speed, 0, 0), true);
	else if (input->isKeyDown(KEY_D))
		m_camera.translate(Vector3(speed, 0, 0), true);

	if (input->isKeyDown(KEY_F))
		m_camera.translate(Vector3(0, speed, 0), true);
	else if (input->isKeyDown(KEY_C))
		m_camera.translate(Vector3(0, -speed, 0), true);

	if (input->isKeyDown(KEY_E))
		m_camera.rotate(Vector3(0, 0, 1), -speed);
	else if (input->isKeyDown(KEY_Q))
		m_camera.rotate(Vector3(0, 0, 1), speed);

	if (input->getMouse().isKeyDown(MOUSE_BUTTON_RIGHT))
	{
		Vector2 direction = input->getMouse().getDirection();
		m_camera.rotate(Vector3(1, 0, 0), -direction.y * dt * 10);
		m_camera.rotate(Vector3(0, 1, 0), -direction.x * dt * 10);
	}

	m_camera.updateMatrix();
	m_overlayScene->update();

	for(size_t i = 0; i < m_overlayScene->getObjectsNumber(); i++)
	{
		auto object = m_overlayScene->getObjectByIndex(i);
		Vector3 position = object->getPosition();

		object->getMatrix()->lookAt(object->getTransformedPosition(), m_camera.getTransformedPosition(), Vector3(0,0,1));
		Vector3 rotation = object->getMatrix()->getEulerAngles();

		object->setEulerRotation(rotation);
		object->setPosition(position);
		object->updateMatrix();
	}
}

void SceneView::updateOverlayScene()
{
	m_overlayScene = make_shared<Scene>();

	auto engine = NeoEngine::getInstance();
	auto level = engine->getLevel();
	auto scene = level->getCurrentScene();

	if(!scene)
		return;

	for(size_t i = 0; i < scene->getObjectsNumber(); i++)
	{
		Object3d* object = scene->getObjectByIndex(i);
		switch(object->getType())
		{
			case OBJECT3D_CAMERA:
			{
				auto entity = m_overlayScene->addNewEntity(level->loadMesh("data/objects/camera.dae"));
				entity->linkTo(object);
			}
				break;

			case OBJECT3D_LIGHT:
			{
				auto entity = m_overlayScene->addNewEntity(level->loadMesh("data/objects/light.dae"));
				entity->linkTo(object);
			}
				break;

			case OBJECT3D_SOUND:
			{
				auto entity = m_overlayScene->addNewEntity(level->loadMesh("data/objects/sound.dae"));
				entity->linkTo(object);
			}
				break;
		}
	}
}

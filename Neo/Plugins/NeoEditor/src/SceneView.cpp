#include "SceneView.h"
#include <CommonEvents.h>
#include <algorithm>

using namespace Neo;

SceneView::SceneView(UndoQueue& undo,
					 int x,
					 int y,
					 unsigned int w,
					 unsigned int h,
					 const shared_ptr<Neo2D::Object2D>& parent)
	: Widget(x, y, w, h, nullptr, parent, nullptr),
	  m_currentHandles(&m_translation),
	  m_undo(undo)
{
	resetCamera();

	registerEvent(make_shared<Neo2D::Gui::MouseDeselectEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseLeftClickEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseRightClickEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseMoveEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseLeftReleaseEvent>(*this, nullptr, nullptr));

	updateOverlayScene();
}

void SceneView::setHandleMode(HANDLE_MODE mode)
{
	m_mode = mode;
	m_currentHandles->enable(false);
	switch (mode)
	{
		case TRANSLATION:
			m_currentHandles = &m_translation;
			break;

		case SCALE:
			m_currentHandles = &m_scale;
			break;

		case ROTATION:
			m_currentHandles = &m_rotation;
			break;
	}

	if (m_selection.size() > 0)
	{
		m_currentHandles->enable(true);
		m_currentHandles->setPosition(getSelectionCenter());
	}
}

void SceneView::resetCamera()
{
	m_camera.setPosition(Vector3(0,0,0));
	m_camera.setEulerRotation(Vector3(90,0,0));
	m_camera.setClippingFar(100000);
	m_camera.setFov(75);
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
	m_handlesScene->draw(&m_camera);

	render->disableDepthTest();
	render->disableScissorTest();
	engine->getRenderer()->set2D(size);
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

	m_currentHandles->setPosition(getSelectionCenter());
	m_currentHandles->enable(true);
}

bool SceneView::handle(const Neo2D::Gui::Event& e)
{
	switch(e.getType())
	{
		case Neo2D::Gui::MOUSE_RIGHT_CLICK:
			setState(Neo2D::Gui::WIDGET_SELECTED);
			return true;

		case Neo2D::Gui::MOUSE_LEFT_RELEASE:
			if(m_currentHandles->grabbed)
				m_undo.save();

			return true;

		case Neo2D::Gui::MOUSE_MOVED:	
		case Neo2D::Gui::MOUSE_LEFT_CLICK:
		{
			if(e.getType() == Neo2D::Gui::MOUSE_LEFT_CLICK)
			{
				setState(Neo2D::Gui::WIDGET_SELECTED);
				m_currentHandles->grabbed = nullptr;
			}
			
			NeoEngine* engine = NeoEngine::getInstance();
			Scene* scene = engine->getLevel()->getCurrentScene();

			InputContext* input = engine->getInputContext();
			Vector2 screen = engine->getSystemContext()->getScreenSize();
			Vector2 mousepos = input->getMouse().getPosition();

			mousepos.y = screen.y - mousepos.y;
			Vector3 cameraAxis = m_camera.getRotatedVector(Vector3(0, 0, -1)).getNormalized();

			Vector3 origin = m_camera.getTransformedPosition();
			Vector3 direction = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 1));

			if(m_camera.isOrtho())
			{
				Vector3 point;
				isRayPlaneIntersection(direction, -cameraAxis, origin + (cameraAxis * m_camera.getClippingNear()), cameraAxis, &point);
				origin = point;
			}

			direction = origin + ((direction - origin).getNormalized() * (m_camera.getClippingFar() - m_camera.getClippingNear()));

			Object3d* selected = m_currentHandles->grabbed;
			float selectedDistance = 0.0f;

			if(m_currentHandles->grabbed == nullptr)
			{
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

				for(size_t i = 0; i < scene->getTextsNumber(); i++)
				{
					auto text = scene->getTextByIndex(i);
	
					auto box = text->getBoundingBox();
					auto inverse = text->getMatrix()->getInverse();

					if(isEdgeToBoxCollision(inverse * origin, inverse * direction, box->min, box->max))
					{
						if(float newlength = (origin - text->getTransformedPosition()).getLength() > selectedDistance
						   || selected == nullptr)
						{
							selected = text;
							selectedDistance = newlength;
						}
					}
				}
			
				// Search in overlay scene
				for(size_t i = 0; i < m_overlayScene->getEntitiesNumber(); i++)
				{
					// Ignore inactive parts
					if(!m_overlayScene->getEntityByIndex(i)->isActive())
						continue;

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

				for(size_t i = 0; i < m_handlesScene->getEntitiesNumber(); i++)
				{
					// Ignore inactive parts
					if(!m_handlesScene->getEntityByIndex(i)->isActive())
						continue;

					auto result = m_handlesScene->getEntityByIndex(i)->castRay(origin, direction);
					if(result.hit)
					{
						if(float newlength = (origin - result.hit).getLength() > selectedDistance
						   || selected == nullptr)
						{
							selected = m_handlesScene->getEntityByIndex(i);
							selectedDistance = newlength;
						}
					}
				}
			}
			
			if(selected)
			{
				// Handle handles
				if(input->getMouse().isKeyDown(MOUSE_BUTTON_LEFT))
				{
					const float rotationSpeed = 2.0;
					if(selected == m_currentHandles->x)
					{
						//local p1 = camera:getUnProjectedPoint(NeoLua.Vector3(mx, my, 0.3))
						//local p2 = camera:getUnProjectedPoint(NeoLua.Vector3(Editor.mx * res.x, (1 - Editor.my) * res.y, 0.3))
						//local dif = (p1 - p2) * 100

						if(m_mode == TRANSLATION)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(1, 0, 0);

							for(auto e : m_selection)
								e->translate(vec);

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->x;
						}
						else if(m_mode == SCALE)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(1, 0, 0) * 0.007;

							for(auto e : m_selection)
								e->setScale(e->getScale() + e->getRotatedVector(vec));

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->x;
						}
						else if(m_mode == ROTATION)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(0, 0, 1);
							vec.x = vec.z * rotationSpeed;
							vec.z = 0;

							for(auto e : m_selection)
								e->setEulerRotation(e->getEulerRotation() - vec);

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->x;
						}
					}
					else if(selected == m_currentHandles->y)
					{					
						if(m_mode == TRANSLATION)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(0, 1, 0);

							for(auto e : m_selection)
								e->translate(vec);

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->y;
						}
						else if(m_mode == SCALE)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(0, 1, 0) * -0.007;						
							for(auto e : m_selection)
								e->setScale(e->getScale() + e->getRotatedVector(vec));

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->y;
						}
						else if(m_mode == ROTATION)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(0, 0, 1);
							vec.y = vec.z * rotationSpeed;
							vec.z = 0;

							for(auto e : m_selection)
								e->setEulerRotation(e->getEulerRotation() - vec);

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->y;
						}
					}
					else if(selected == m_currentHandles->z)
					{					
						if(m_mode == TRANSLATION)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(0, 0, 1);

							for(auto e : m_selection)
								e->translate(vec);

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->z;
						}
						else if(m_mode == SCALE)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(0, 0, 1) * 0.015;

							for(auto e : m_selection)
								e->setScale(e->getScale() + e->getRotatedVector(vec));

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->z;
						}
						else if(m_mode == ROTATION)
						{
							Vector2 mousedir = input->getMouse().getDirection();
							mousedir.y *= -1;

							Vector2 oldpos = mousepos - mousedir;

							float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
							Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
							Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

							Vector3 vec = (p1 - p2) * distance * Vector3(1, 0, 0);
							vec.z = vec.x * rotationSpeed;
							vec.x = 0;

							for(auto e : m_selection)
								e->setEulerRotation(e->getEulerRotation() + vec);

							m_currentHandles->setPosition(getSelectionCenter());
							m_currentHandles->grabbed = m_currentHandles->z;
						}
					}
					else if(e.getType() == Neo2D::Gui::MOUSE_LEFT_CLICK)
					{	
						if(!input->isKeyDown(Neo::KEY_LSHIFT))
							clearSelection();

						addSelectedObject(selected);
						doCallback();
					}
				}
			}
			else if(e.getType() == Neo2D::Gui::MOUSE_LEFT_CLICK)
				m_currentHandles->enable(false);
		}
			return true;

		case Neo2D::Gui::MOUSE_DESELECT:
			setState(Neo2D::Gui::WIDGET_NORMAL);
			return true;
	}

	return false;
}

void SceneView::update(float dt)
{
	Vector3 scale = (m_camera.getPosition() - getSelectionCenter()).getLength() * 0.0075;
	m_currentHandles->setScale(scale);
	m_currentHandles->setPosition(getSelectionCenter());
	
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
	m_handlesScene->update();
	
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
	MLOG_INFO("Updating overlay scene");

	if(!m_overlayScene)
		m_overlayScene = make_shared<Scene>();
	
	auto engine = NeoEngine::getInstance();
	auto level = engine->getLevel();
	auto scene = level->getCurrentScene();	
	
	if(!scene)
		return;

	for(size_t i = 0; i < m_overlayScene->getObjectsNumber(); i++)
		m_overlayScene->deleteObject(m_overlayScene->getObjectByIndex(i));

	// Load handles
	if(!m_handlesScene)
	{		
		m_handlesScene = make_shared<Scene>();
		
		// The arrows!
		MeshRef* meshref = level->loadMesh("data/arrow.obj");
		m_translation.x = m_handlesScene->addNewEntity(meshref);
		m_translation.y = m_handlesScene->addNewEntity(meshref);
		m_translation.z = m_handlesScene->addNewEntity(meshref);

		m_translation.x->getMaterial(0)->setEmit(Vector3(1, 0, 0));
		m_translation.y->getMaterial(0)->setEmit(Vector3(0, 1, 0));
		m_translation.z->getMaterial(0)->setEmit(Vector3(0, 0, 1));

		m_translation.x->rotate(Vector3(0, 1, 0), 90);
		m_translation.y->rotate(Vector3(1, 0, 0), 90);

		// The scalers!
		meshref = level->loadMesh("data/scale-arrow.obj");
		m_scale.x = m_handlesScene->addNewEntity(meshref);
		m_scale.y = m_handlesScene->addNewEntity(meshref);
		m_scale.z = m_handlesScene->addNewEntity(meshref);

		m_scale.x->getMaterial(0)->setEmit(Vector3(1, 0, 0));
		m_scale.y->getMaterial(0)->setEmit(Vector3(0, 1, 0));
		m_scale.z->getMaterial(0)->setEmit(Vector3(0, 0, 1));

		m_scale.x->rotate(Vector3(0, 1, 0), 90);
		m_scale.y->rotate(Vector3(1, 0, 0), 90);

		meshref = level->loadMesh("data/rotation-arrow.obj");
		m_rotation.x = m_handlesScene->addNewEntity(meshref);
		m_rotation.y = m_handlesScene->addNewEntity(meshref);
		m_rotation.z = m_handlesScene->addNewEntity(meshref);

		m_rotation.x->getMaterial(0)->setEmit(Vector3(1, 0, 0));
		m_rotation.y->getMaterial(0)->setEmit(Vector3(0, 1, 0));
		m_rotation.z->getMaterial(0)->setEmit(Vector3(0, 0, 1));

		m_rotation.x->rotate(Vector3(0, 1, 0), 90);
		m_rotation.y->rotate(Vector3(1, 0, 0), 90);

		m_translation.enable(false);
		m_scale.enable(false);
		m_rotation.enable(false);

		setHandleMode(TRANSLATION);
	}

	// Load billboards
	for (size_t i = 0; i < scene->getObjectsNumber(); i++)
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

Vector3 SceneView::getSelectionCenter()
{
	Vector3 center;
	for(auto e : m_selection)
	{
		center += e->getTransformedPosition();
	}

	return center / m_selection.size();
}

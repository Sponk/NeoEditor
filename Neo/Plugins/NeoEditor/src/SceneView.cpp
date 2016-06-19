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
	  m_undo(undo),
	  m_objectLocalTransformation(false)
{
	m_overlayScene = m_level.addNewScene();
	m_handlesScene = m_level.addNewScene();
}

void SceneView::init()
{
	resetCamera();

	registerEvent(make_shared<Neo2D::Gui::MouseDeselectEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseLeftClickEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseRightClickEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseMoveEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseLeftReleaseEvent>(shared_from_this(), nullptr, nullptr));

	NeoEngine* engine = NeoEngine::getInstance();
	Level* level = engine->getLevel();
	engine->setLevel(&m_level);

	{
		// The arrows!
		MeshRef* meshref = m_level.loadMesh("data/arrow.obj");
		m_translation.x = m_handlesScene->addNewEntity(meshref);
		m_translation.y = m_handlesScene->addNewEntity(meshref);
		m_translation.z = m_handlesScene->addNewEntity(meshref);

		m_translation.x->getMaterial(0)->setEmit(Vector3(1, 0, 0));
		m_translation.y->getMaterial(0)->setEmit(Vector3(0, 1, 0));
		m_translation.z->getMaterial(0)->setEmit(Vector3(0, 0, 1));

		m_translation.x->rotate(Vector3(0, 1, 0), 90);
		m_translation.y->rotate(Vector3(1, 0, 0), -90);

		// The scalers!
		meshref = m_level.loadMesh("data/scale-arrow.obj");
		m_scale.x = m_handlesScene->addNewEntity(meshref);
		m_scale.y = m_handlesScene->addNewEntity(meshref);
		m_scale.z = m_handlesScene->addNewEntity(meshref);

		m_scale.x->getMaterial(0)->setEmit(Vector3(1, 0, 0));
		m_scale.y->getMaterial(0)->setEmit(Vector3(0, 1, 0));
		m_scale.z->getMaterial(0)->setEmit(Vector3(0, 0, 1));

		m_scale.x->rotate(Vector3(0, 1, 0), 90);
		m_scale.y->rotate(Vector3(1, 0, 0), -90);

		meshref = m_level.loadMesh("data/rotation-arrow.obj");
		m_rotation.x = m_handlesScene->addNewEntity(meshref);
		m_rotation.y = m_handlesScene->addNewEntity(meshref);
		m_rotation.z = m_handlesScene->addNewEntity(meshref);

		m_rotation.x->getMaterial(0)->setEmit(Vector3(1, 0, 0));
		m_rotation.y->getMaterial(0)->setEmit(Vector3(0, 1, 0));
		m_rotation.z->getMaterial(0)->setEmit(Vector3(0, 0, 1));

		m_rotation.x->rotate(Vector3(0, 1, 0), 90);
		m_rotation.y->rotate(Vector3(1, 0, 0), -90);

		m_translation.enable(false);
		m_scale.enable(false);
		m_rotation.enable(false);

		setHandleMode(TRANSLATION);
	}
	engine->setLevel(level);
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
	m_camera.setClearColor(Vector4(0.223529411764706,0.223529411764706,0.223529411764706,1));
}

void SceneView::draw(const Neo::Vector2& offset)
{	
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	
	const Vector2 size = engine->getSystemContext()->getScreenSize();
	render->setViewport(0, 0, size.x, size.y);
	render->setClearColor(m_camera.getClearColor());
	render->clear(BUFFER_COLOR | BUFFER_DEPTH);
	render->enableScissorTest();
	render->setScissor(getPosition().x, size.y - (getPosition().y + getSize().y), getSize().x, getSize().y);

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
	if(!object)
		return;

	m_selection.push_back(object);

	if(object->getType() == OBJECT3D_ENTITY)
		static_cast<OEntity*>(object)->enableWireframe(true);

	m_currentHandles->setPosition(getSelectionCenter());
	m_currentHandles->enable(true);
}

void SceneView::rotationHandle(OEntity* handleEntity, const Vector3& axis, const Vector3& mousepos)
{	
	const float rotationSpeed = 2;
	InputContext* input = NeoEngine::getInstance()->getInputContext();
	Vector2 mousedir = input->getMouse().getDirection();
	mousedir.y *= -1;

	Vector2 oldpos = mousepos - mousedir;

	float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
	Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
	Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

	if(isObjectLocal())
	{
		Vector3 vec = (p1 - p2) * distance * axis * rotationSpeed;
		for(auto e : m_selection)
			e->setEulerRotation(e->getEulerRotation() + vec);
	}
	else
	{
		Vector3 selectionCenter = getSelectionCenter();
		Vector3 movement = p1 - p2;
		Vector3 movementAxis = movement * axis;
		float movementLength = movement.getLength();
		for(auto e : m_selection)
		{
			// Update the rotation
			Matrix4x4 matrix;
			Vector3 inverseCenter = e->getInversePosition(selectionCenter);
			Quaternion rotation = e->getRotation();
			matrix.setRotationAxis(rotation.getAngle(), rotation.getAxis());
			matrix.invert();

			Vector3 newAxis = matrix * movementAxis;
			e->addAxisAngleRotation(newAxis, movementLength * distance * rotationSpeed);
			e->updateMatrix();

			// Update the translation
			Vector3 transformedCenter = e->getTransformedVector(inverseCenter);
			e->setPosition(e->getPosition() + (selectionCenter - transformedCenter));
			e->updateMatrix();
		}
	}
	
	m_currentHandles->setPosition(getSelectionCenter());
	m_currentHandles->grabbed = handleEntity;
}

void SceneView::scaleHandle(OEntity* handleEntity, const Vector3& axis, const Vector3& mousepos)
{
	InputContext* input = NeoEngine::getInstance()->getInputContext();
	Vector2 mousedir = input->getMouse().getDirection();
	mousedir.y *= -1;

	Vector2 oldpos = mousepos - mousedir;

	float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
	Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
	Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

	float factor = distance * 0.01;

	Vector3 vec = (p1 - p2) * axis * factor;

	for(auto e : m_selection)
	{
		Vector3 newScale = e->getTransformedScale();
		Matrix4x4 scaleMatrix;
		scaleMatrix.setScale(Vector3(1.0f / newScale.x, 1.0f / newScale.y, 1.0f / newScale.z));

        Matrix4x4 matrix = (*e->getMatrix()) * scaleMatrix;
		Vector3 transformedVec = matrix.getRotatedVector3(vec);

		newScale = e->getScale() + transformedVec * axis * factor;
		e->setScale(Vector3(fabs(newScale.x), fabs(newScale.y), fabs(newScale.z)));
	}
	
	m_currentHandles->setPosition(getSelectionCenter());
	m_currentHandles->grabbed = handleEntity;
}

void SceneView::translationHandle(OEntity* handleEntity, const Vector3& axis, const Vector3& mousepos)
{
	InputContext* input = NeoEngine::getInstance()->getInputContext();
	Vector2 mousedir = input->getMouse().getDirection();
	mousedir.y *= -1;

	Vector2 oldpos = mousepos - mousedir;

	float distance = (m_camera.getPosition() - getSelectionCenter()).getLength();
	Vector3 p1 = m_camera.getUnProjectedPoint(Vector3(mousepos.x, mousepos.y, 0));
	Vector3 p2 = m_camera.getUnProjectedPoint(Vector3(oldpos.x, oldpos.y, 0));

	Vector3 vec = (p1 - p2) * distance * axis;

	for(auto e : m_selection)
		e->translate(vec);

	m_currentHandles->setPosition(getSelectionCenter());
	m_currentHandles->grabbed = handleEntity;				
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

		case Neo2D::Gui::MOUSE_MOVE:	
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
					if(selected == m_currentHandles->x)
					{
						if(m_mode == TRANSLATION)
							translationHandle(m_currentHandles->x, Vector3(1,0,0), mousepos);
						
						else if(m_mode == SCALE)
							scaleHandle(m_currentHandles->x, Vector3(1,0,0), mousepos);
						
						else if(m_mode == ROTATION)
							rotationHandle(m_currentHandles->x, Vector3(1,0,0), mousepos);
					}
					else if(selected == m_currentHandles->y)
					{					
						if(m_mode == TRANSLATION)
							translationHandle(m_currentHandles->y, Vector3(0,1,0), mousepos);
						
						else if(m_mode == SCALE)
							scaleHandle(m_currentHandles->y, Vector3(0,1,0), mousepos);
						
						else if(m_mode == ROTATION)
							rotationHandle(m_currentHandles->y, Vector3(0,1,0), mousepos);
						
					}
					else if(selected == m_currentHandles->z)
					{					
						if(m_mode == TRANSLATION)
							translationHandle(m_currentHandles->z, Vector3(0,0,1), mousepos);

						else if(m_mode == SCALE)
							scaleHandle(m_currentHandles->z, Vector3(0,0,1), mousepos);

						else if(m_mode == ROTATION)
							rotationHandle(m_currentHandles->z, Vector3(0,0,1), mousepos);
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
			{
				clearSelection();
				m_currentHandles->enable(false);
			}
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
	Neo2D::Gui::Widget::update(dt);
	Vector3 scale = (m_camera.getPosition() - getSelectionCenter()).getLength() * 0.0075;
	m_currentHandles->setScale(scale);
	m_currentHandles->setPosition(getSelectionCenter());

    NeoEngine* engine = NeoEngine::getInstance();
    auto input = engine->getInputContext();

    if(getState() != Neo2D::Gui::WIDGET_SELECTED
            || input->isKeyDown(KEY_LCONTROL)) // So Ctrl + * shortcuts don't move the camera
		return;

	float speed = 60.0f * dt;
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
	
	auto engine = NeoEngine::getInstance();
	auto level = engine->getLevel();
	auto scene = level->getCurrentScene();	
	
	if(!scene)
		return;

	clearOverlayScene();

	auto cameraEntity = m_level.loadMesh("data/objects/camera.dae");
	auto lightEntity = m_level.loadMesh("data/objects/light.dae");
	auto soundEntity = m_level.loadMesh("data/objects/sound.dae");

	// Load billboards
	for (size_t i = 0; i < scene->getObjectsNumber(); i++)
	{
		Object3d* object = scene->getObjectByIndex(i);
		switch(object->getType())
		{
			case OBJECT3D_CAMERA:
			{
				auto entity = m_overlayScene->addNewEntity(cameraEntity);
				entity->setParent(object);
			}
				break;

			case OBJECT3D_LIGHT:
			{
				auto entity = m_overlayScene->addNewEntity(lightEntity);
				entity->setParent(object);
			}
				break;

			case OBJECT3D_SOUND:
			{
				auto entity = m_overlayScene->addNewEntity(soundEntity);
				entity->setParent(object);
			}
				break;
		}
	}
	engine->setLevel(level);
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

void SceneView::clearOverlayScene()
{
	for(size_t i = 0; i < m_overlayScene->getObjectsNumber(); i++)
	{
		auto object = m_overlayScene->getObjectByIndex(i);
		object->setParent(nullptr);
		m_overlayScene->deleteObject(object);
	}
}

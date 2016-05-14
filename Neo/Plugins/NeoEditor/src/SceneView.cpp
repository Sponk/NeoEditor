#include "SceneView.h"
#include <CommonEvents.h>

using namespace Neo;

SceneView::SceneView(int x,
					 int y,
					 unsigned int w,
					 unsigned int h,
					 const shared_ptr<Neo2D::Object2D>& parent)
	: Widget(x, y, w, h, nullptr, parent, nullptr)
{
	resetCamera();

	//registerEvent(make_shared<Neo2D::Gui::MouseMoveEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseDeselectEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseLeftClickEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<Neo2D::Gui::MouseRightClickEvent>(*this, nullptr, nullptr));

	//registerEvent(make_shared<Neo2D::Gui::MouseRightReleaseEvent>(*this, nullptr, nullptr));
	//registerEvent(make_shared<Neo2D::Gui::MouseLeftReleaseEvent>(*this, nullptr, nullptr));
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
	render->disableScissorTest();
}

void SceneView::handle(const Neo2D::Gui::Event& e)
{
	switch(e.getType())
	{
		case Neo2D::Gui::MOUSE_RIGHT_CLICK:
		case Neo2D::Gui::MOUSE_LEFT_CLICK:
			setState(Neo2D::Gui::WIDGET_SELECTED);
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
}

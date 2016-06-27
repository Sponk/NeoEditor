#include "Sidepanel.h"

void Sidepanel::update(float dt)
{
	Neo::Vector2 screen = Neo::NeoEngine::getInstance()->getSystemContext()->getScreenSize();
	switch(edge)
	{
		case LEFT_EDGE:
			setSize(Neo::Vector2(getSize().x, screen.y - getPosition().y) + sizeOffset);
			break;

		case RIGHT_EDGE:
			setPosition(Neo::Vector2(screen.x - getSize().x - 15, getPosition().y));
			setSize(Neo::Vector2(getSize().x, screen.y - getPosition().y) + sizeOffset);
			break;

		case TOP_EDGE:
			setSize(Neo::Vector2(screen.x - getPosition().x, getSize().y) + sizeOffset);
			break;

		case BOTTOM_EDGE:
			setPosition(Neo::Vector2(getPosition().x, screen.y - getSize().y));
			setSize(Neo::Vector2(screen.x - getPosition().x, getSize().y) + sizeOffset);
			break;
	}

	updateLayout();
	Neo2D::Gui::Container::update(dt);
}

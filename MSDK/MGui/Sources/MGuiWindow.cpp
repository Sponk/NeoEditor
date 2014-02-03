/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiWindow.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include "../Includes/MGui.h"


//constructor
MGuiWindow::MGuiWindow(const MVector2 & position, const MVector2 & scale, const MVector4 & color, void (* pointerEvent)(MGuiWindow * window, MGuiEvent * guiEvent)):
m_breakEvents(false),
m_margin(10, 10),
m_isScrolled(false),
m_scroll(0, 0),
m_minScroll(0, 0),
m_maxScroll(0, 0),
m_hScrollSlide(MVector2(0, scale.y-6), MVector2(50, 6), MVector4(1, 1, 1, 0.3f), NULL),
m_vScrollSlide(MVector2(scale.x-6, 0), MVector2(6, 50), MVector4(1, 1, 1, 0.3f), NULL)
{
	m_hScrollSlide.setParentWindow(this);
	m_vScrollSlide.setParentWindow(this);

	setParentWindow(NULL);
	setPosition(position);
	setScale(scale);
	setNormalColor(color);
	setPointerEvent(pointerEvent);

	// horizontal slide
	m_hScrollSlide.setScrollAffect(false);
	m_hScrollSlide.setMinValue(0);
	m_hScrollSlide.setMaxValue(1);

	// vertical slide
	m_vScrollSlide.setScrollAffect(false);
	m_vScrollSlide.setMinValue(0);
	m_vScrollSlide.setMaxValue(1);

	resizeScroll();
}

MGuiWindow::~MGuiWindow(void)
{
	clear();
}

void MGuiWindow::clear(void)
{
	unsigned int i;

	unsigned int oSize = m_objects.size();
	for(i=0; i<oSize; i++)
		SAFE_DELETE(m_objects[i]);

	m_objects.clear();
	m_menus.clear();
	m_buttons.clear();
	m_texts.clear();
	m_images.clear();
	m_editTexts.clear();
	m_slides.clear();

	m_breakEvents = true;
	//resizeScroll();
}

bool MGuiWindow::isSomethingEditing(void)
{
	unsigned int i;
	unsigned int eSize = m_editTexts.size();
	for(i=0; i<eSize; i++){
		if(m_editTexts[i]->isPressed())
			return true;
	}

	return false;
}

void MGuiWindow::autoScale(void)
{
	MVector2 tmp;
	MVector2 newScale(0, 0);

    unsigned int i;
    unsigned int size = m_objects.size();
    for(i=0; i<size; i++)
	{
		MGui2d * object = m_objects[i];
		tmp = object->getPosition()
		    + object->getScale();

		if(tmp.x > newScale.x) newScale.x = tmp.x;
		if(tmp.y > newScale.y) newScale.y = tmp.y;
	}

	setScale(newScale);
	resizeScroll();
}

void MGuiWindow::addButton(MGuiButton * button){
	button->setParentWindow(this);
	m_buttons.push_back(button);
	m_objects.push_back(button);
}

void MGuiWindow::addMenu(MGuiMenu * menu){
	menu->setParentWindow(this);
	m_menus.push_back(menu);
	m_objects.push_back(menu);
}

void MGuiWindow::addText(MGuiText * text){
	text->setParentWindow(this);
	m_texts.push_back(text);
	m_objects.push_back(text);
}

void MGuiWindow::addImage(MGuiImage * image){
	image->setParentWindow(this);
	m_images.push_back(image);
	m_objects.push_back(image);
}

void MGuiWindow::addEditText(MGuiEditText * editText){
	editText->setParentWindow(this);
	m_editTexts.push_back(editText);
	m_objects.push_back(editText);
}

void MGuiWindow::addSlide(MGuiSlide * slide){
	slide->setParentWindow(this);
	m_slides.push_back(slide);
	m_objects.push_back(slide);
}

bool MGuiWindow::isScrollBarPressed(void)
{
	return (m_hScrollSlide.isPressed() || m_vScrollSlide.isPressed());
}

void MGuiWindow::internalEvent(MWinEvent * windowEvent)
{
	unsigned int i;

	// scolling slides
	if(isHorizontalScroll())
		m_hScrollSlide.onEvent(windowEvent);

	if(isVerticalScroll())
		m_vScrollSlide.onEvent(windowEvent);

	if(isScrollBarPressed()) // disable gui events if scroll bar pressed
		return;

	if(m_breakEvents)
		m_breakEvents = false;

	unsigned int oSize = m_objects.size();
	for(i=0; i<oSize; i++)
	{
		if(m_breakEvents)
			break;

		if(m_objects[i]->isVisible())
			m_objects[i]->onEvent(windowEvent);
	}
}

int MGuiWindow::onWindowMenusEvent(MWinEvent * windowEvent)
{
	unsigned int i;
	unsigned int mSize = m_menus.size();

	for(i=0; i<mSize; i++)
	{
		if(m_menus[i]->isVisible() && m_menus[i]->isPressed())
		{
			m_menus[i]->onWindowMenuEvent(windowEvent);
			m_menus[i]->onEvent(windowEvent);

			return 1;
		}
	}

	return 0;
}

void MGuiWindow::onEvent(MWinEvent * windowEvent)
{
	MMouse * mouse = MMouse::getInstance();

	switch(windowEvent->type)
	{
	case MWIN_EVENT_WINDOW_RESIZE:
		if(m_pointerEvent) // send gui event
		{
			MGuiEvent guiEvent;
			guiEvent.type = MGUI_EVENT_RESIZE;
			m_pointerEvent(this, &guiEvent);
		}
		resizeScroll();
		break;
	case MWIN_EVENT_KEY_DOWN:
		switch(windowEvent->data[0])
		{
			case MKEY_TAB:
				{
					// editTexts
					unsigned int eSize = m_editTexts.size();
					unsigned int i;

					// select next edit text
					for(i=0; i<eSize; i++)
					{
						if(m_editTexts[i]->isSingleLine() && m_editTexts[i]->isPressed())
						{
							m_editTexts[i]->setPressed(false);
							m_editTexts[i]->sendVariable();
							unsigned int nextId = i + 1;

							if(nextId >= eSize)
								nextId = 0;

							m_editTexts[nextId]->setPressed(true);
							m_editTexts[nextId]->setCharId(0);
							m_editTexts[nextId]->setSelection(0, strlen(m_editTexts[nextId]->getText()));

							return;
						}
					}

					break;
				}
			case MKEY_DOWN:
				{
					// editTexts
					unsigned int eSize = m_editTexts.size();
					unsigned int i;

					// select next edit text
					for(i=0; i<eSize; i++)
					{
						if(m_editTexts[i]->isSingleLine() && m_editTexts[i]->isPressed())
						{
							m_editTexts[i]->setPressed(false);
							m_editTexts[i]->sendVariable();
							unsigned int nextId = i + 1;

							if(nextId >= eSize)
								nextId = 0;

							m_editTexts[nextId]->setPressed(true);
							m_editTexts[nextId]->setCharId(m_editTexts[i]->getCharId());

							return;
						}
					}

					break;
				}
			case MKEY_UP:
				{
					// editTexts
					unsigned int eSize = m_editTexts.size();
					unsigned int i;

					// select previous edit text
					for(i=0; i<eSize; i++)
					{
						if(m_editTexts[i]->isSingleLine() && m_editTexts[i]->isPressed())
						{
							m_editTexts[i]->setPressed(false);
							m_editTexts[i]->sendVariable();
							int nextId = i - 1;

							if(nextId < 0)
								nextId = eSize - 1;

							m_editTexts[nextId]->setPressed(true);
							m_editTexts[nextId]->setCharId(m_editTexts[i]->getCharId());

							return;
						}
					}

					break;
				}
		}
		break;
	case MWIN_EVENT_MOUSE_WHEEL_MOVE:
		if(isHighLight())
			moveScroll(MVector2(0, mouse->getWheelDirection() * (getTextSize() * 3)));

		break;
	case MWIN_EVENT_MOUSE_MOVE:
		if(isScrolled())
			moveScroll(MVector2((float)mouse->getXDirection(), (float)mouse->getYDirection()));

		if(isMouseInside()){
			setHighLight(true);

			if(m_pointerEvent) // send mouse move gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_MOVE;
				guiEvent.data[0] = windowEvent->data[0];
				guiEvent.data[1] = windowEvent->data[1];

				m_pointerEvent(this, &guiEvent);
			}
		}
		else{
		    setHighLight(false);
		}
		break;
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:

		if(isHighLight())
		{
			if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT){
				setPressed(true);
			}

			if(windowEvent->data[0] == MMOUSE_BUTTON_MIDDLE){
				resizeScroll();
				setScrolled(true);
			}
		}
			
		if(m_pointerEvent) // send mouse button down gui event
		{
			MGuiEvent guiEvent;
				
			guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_DOWN;
			guiEvent.data[0] = windowEvent->data[0];
				
			m_pointerEvent(this, &guiEvent);
		}

		break;
	case MWIN_EVENT_MOUSE_BUTTON_UP:

		if(windowEvent->data[0] == MMOUSE_BUTTON_MIDDLE)
			setScrolled(false);

		if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
			setPressed(false);

		if(isHighLight())
		{
			if(m_pointerEvent) // send mouse button up gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_UP;
				guiEvent.data[0] = windowEvent->data[0];

				m_pointerEvent(this, &guiEvent);
			}
		}

		break;
	}

	internalEvent(windowEvent);
}

bool MGuiWindow::isHorizontalScroll(void)
{
	MVector2 scale = getScale();

	float length = (scale.x / (m_maxScroll.x - m_minScroll.x)) * scale.x;

	return (length < scale.x);
}

bool MGuiWindow::isVerticalScroll(void)
{
	MVector2 scale = getScale();

	float height = (scale.y / (m_maxScroll.y - m_minScroll.y)) * scale.y;

	return (height < scale.y);
}

void MGuiWindow::rescaleScrollingBar(void)
{
	MVector2 scale = getScale();

	m_hScrollSlide.setPosition(MVector2(0, scale.y - m_hScrollSlide.getButtonScale().y));
	m_vScrollSlide.setPosition(MVector2(scale.x - m_vScrollSlide.getButtonScale().x, 0));

	float length = (scale.x / (m_maxScroll.x - m_minScroll.x)) * scale.x;

	if(length < scale.x)
	{
		m_hScrollSlide.setButtonScale(MVector2(length, m_hScrollSlide.getButtonScale().y));
		m_hScrollSlide.setDirection(MVector2(scale.x - length - m_vScrollSlide.getButtonScale().x, 0));

		if(isScrollBarPressed())
		{
			float value = m_hScrollSlide.getValue();
			m_scroll.x = -(m_minScroll.x + (m_maxScroll.x - m_minScroll.x - scale.x)*value);
		}
	}

	float height = (scale.y / (m_maxScroll.y - m_minScroll.y)) * scale.y;

	if(height < scale.y)
	{
		m_vScrollSlide.setButtonScale(MVector2(m_vScrollSlide.getButtonScale().x, height));
		m_vScrollSlide.setDirection(MVector2(0, scale.y - height));

		if(isScrollBarPressed())
		{
			float value = m_vScrollSlide.getValue();
			m_scroll.y = -(m_minScroll.y + (m_maxScroll.y - m_minScroll.y - scale.y)*value);
		}
	}
}

void MGuiWindow::limitScroll(void)
{
	MVector2 scale = getScale();

	if(m_scroll.x > -m_minScroll.x)
	   m_scroll.x = -m_minScroll.x;

	if(m_scroll.y > -m_minScroll.y)
	   m_scroll.y = -m_minScroll.y;

	if(m_scroll.x < -(m_maxScroll.x - scale.x))
	   m_scroll.x = -(m_maxScroll.x - scale.x);

	if(m_scroll.y < -(m_maxScroll.y - scale.y))
	   m_scroll.y = -(m_maxScroll.y - scale.y);

	updateScrollingBar();
}

void MGuiWindow::updateScrollingBar(void)
{
	// set scrolling bar value
	float value;

	// horizontal
	value = (-m_scroll.x - m_minScroll.x) / (m_maxScroll.x - m_minScroll.x - getScale().x);
	m_hScrollSlide.setValue(value);

	// vertical
	value = (-m_scroll.y - m_minScroll.y) / (m_maxScroll.y - m_minScroll.y - getScale().y);
	m_vScrollSlide.setValue(value);
}

void MGuiWindow::moveScroll(const MVector2 & direction)
{
	m_scroll.x += direction.x;
	m_scroll.y += direction.y;

	limitScroll();
}

void MGuiWindow::resizeScroll(void)
{
	unsigned int i;

	MVector2 min = getScale();
	MVector2 max = getScale();

	unsigned int oSize = m_objects.size();
	for(i=0; i<oSize; i++)
	{
		MVector2 p1 = m_objects[i]->getPosition();
		MVector2 p2 = p1 + m_objects[i]->getScale();

		min.x = MIN(min.x, p1.x);
		min.y = MIN(min.y, p1.y);

		max.x = MAX(max.x, p2.x);
		max.y = MAX(max.y, p2.y);
	}

	m_minScroll = min;
	m_maxScroll = max;

	if(isHorizontalScroll())
	{
		if(m_minScroll.x < 0)
			m_minScroll.x -= m_margin.x;
		else
			m_minScroll.x = 0;
		m_maxScroll.x += m_margin.x;
	}

	if(isVerticalScroll())
	{
		if(m_minScroll.y < 0)
			m_minScroll.y -= m_margin.y;
		else
			m_minScroll.y = 0;
		m_maxScroll.y += m_margin.y;
	}

	rescaleScrollingBar();

	m_hScrollSlide.onChange();
	m_vScrollSlide.onChange();

	limitScroll();
}

void MGuiWindow::setHighLight(bool highLight)
{
	MGui2d::setHighLight(highLight);

	if(! highLight)
	{
		unsigned int i;
		unsigned int oSize = m_objects.size();
		for(i=0; i<oSize; i++)
			m_objects[i]->setHighLight(false);
	}
}

void MGuiWindow::drawWindowMenus(void)
{
	unsigned int i;
	unsigned int mSize = m_menus.size();
	for(i=0; i<mSize; i++)
	{
		if(m_menus[i]->isVisible() && m_menus[i]->isPressed())
		{
			m_menus[i]->drawWindowMenu();
			return;
		}
	}
}

void MGuiWindow::draw(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	if(! isVisible())
		return;
	
	MWindow * window = MWindow::getInstance();

	rescaleScrollingBar();
	render->enableScissorTest();
	render->setScissor((int)getPosition().x, window->getHeight() - (int)getPosition().y - (unsigned int)getScale().y, (unsigned int)getScale().x, (unsigned int)getScale().y);

	// normal clear
	if((getNormalColor().w >= 1.0f) && (! hasNormalTexture()))
	{
		render->setClearColor(getNormalColor());
		render->clear(M_BUFFER_COLOR);

		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_DRAW;

		if(m_pointerEvent)
			m_pointerEvent(this, &guiEvent);

		// 2d mode
		set2dMode(render);
		render->disableDepthTest();
		render->disableCullFace();
		render->disableLighting();
		render->enableBlending();
		render->setBlendingMode(M_BLENDING_ALPHA);
		render->enableTexture();
	}
	else
	{
		// background
		set2dMode(render);
		render->disableDepthTest();
		render->disableCullFace();
		render->disableLighting();
		render->enableBlending();
		render->setBlendingMode(M_BLENDING_ALPHA);

		if(hasNormalTexture()) // texture clear
		{
			render->enableTexture();
			render->setColor4(getNormalColor());
			drawTexturedQuad(getNormalTexture());
		}
		else if(getNormalColor().w < 1.0f)
		{
			render->disableTexture();
			render->setColor4(getNormalColor());
			drawQuad();
		}

		if(m_pointerEvent)
		{
			MGuiEvent guiEvent;
			guiEvent.type = MGUI_EVENT_DRAW;
			m_pointerEvent(this, &guiEvent);

			// 2d mode
			set2dMode(render);
			render->disableDepthTest();
			render->disableCullFace();
			render->disableLighting();
			render->enableBlending();
			render->setBlendingMode(M_BLENDING_ALPHA);
			render->enableTexture();
		}
	}

	// gui
	render->pushMatrix();
	render->translate(MVector3(getPosition().x, getPosition().y, 0));

	render->pushMatrix();
	render->translate(MVector3(getScroll().x, getScroll().y, 0));

	// drawing
	unsigned int i;
	unsigned int oSize = m_objects.size();
	for(i=0; i<oSize; i++)
		m_objects[i]->draw();

	render->popMatrix();

	// draw shadows
	if(hasShadow())
	{
		render->disableScissorTest();
		drawShadow();
		render->enableScissorTest();
	}

	// scolling slides
	if(isHorizontalScroll())
		m_hScrollSlide.draw();

	if(isVerticalScroll())
		m_vScrollSlide.draw();

	render->popMatrix();
}

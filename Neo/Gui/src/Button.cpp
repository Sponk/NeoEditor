/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of NeoGui.
 *
 * NeoGui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NeoGui is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with NeoGui.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von NeoGui.
 *
 * NeoGui ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * NeoGui wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <Button.h>
#include <Render.h>
#include <GuiSystem.h>
#include <MMouse.h>

using namespace Neo;

Button::Button(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char* label) :
    Widget(x,y,width,height,label),
    m_labelText(NULL),
    m_state(BUTTON_NORMAL_STATE)
{

}

void Button::draw()
{
    Render* render = Render::getInstance();
    MSystemContext* system = MEngine::getInstance()->getSystemContext();

    if(m_labelText == NULL)
    {
        char file[256];
        getGlobalFilename(file, system->getWorkingDirectory(), GuiSystem::getInstance()->getDefaultFont());
        m_labelText = new MOText(MEngine::getInstance()->getLevel()->loadFont(file));
        m_labelText->setSize(GuiSystem::getInstance()->getDefaultFontSize());
        m_labelText->setAlign(M_ALIGN_CENTER);
    }

    m_labelText->setText(m_label.c_str());

    MVector4 color;
    switch(m_state)
    {
        case BUTTON_NORMAL_STATE:
            color = GuiSystem::getInstance()->getNormalBackground();
        break;

    case BUTTON_HOVER_STATE:
            color = GuiSystem::getInstance()->getHoverBackground();
        break;

    case BUTTON_PRESSED_STATE:
            color = GuiSystem::getInstance()->getHighlightBackground();
        break;
    }

    render->drawColoredQuad(m_x, m_y, m_width, m_height, color);
    render->drawText(m_labelText, m_x + 0.5*m_width, m_y + 0.5*m_labelText->getSize() + 0.5*m_height);
}

void Button::update()
{
    MMouse* mouse = MMouse::getInstance();

    unsigned int x = mouse->getXPosition();
    unsigned int y = mouse->getYPosition();

    if(m_state == BUTTON_PRESSED_STATE && !mouse->isLeftButtonPushed())
    {
        doCallback();
    }

    if(x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y + m_height)
    {
        m_state = BUTTON_HOVER_STATE;
    }
    else
    {
        m_state = BUTTON_NORMAL_STATE;
    }

    if(m_state == BUTTON_HOVER_STATE && mouse->isLeftButtonPushed())
    {
        m_state = BUTTON_PRESSED_STATE;
    }
}

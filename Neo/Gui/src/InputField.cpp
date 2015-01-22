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

#include <InputField.h>
#include <Render.h>
#include <GuiSystem.h>
#include <Window/Keyboard.h>
#include <Window/Mouse.h>
#include <Window/WinEvents.h>

using namespace Neo::Gui;

InputField::InputField(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char* label) :
    Widget(x,y,width,height,label),
    m_labelText(NULL),
    m_state(INPUT_NORMAL_STATE)
{

}

void InputField::draw()
{
    Render* render = Render::getInstance();
    MSystemContext* system = NeoEngine::getInstance()->getSystemContext();
    GuiSystem* gui = GuiSystem::getInstance();

    unsigned int winh, winw;
    system->getScreenSize(&winw, &winh);

    if(m_labelText == NULL)
    {
        m_labelText = render->createText(gui->getDefaultFont(), gui->getDefaultFontSize());
        m_labelText->setAlign(TEXT_ALIGN_LEFT);
    }

    MVector4 color;
    switch(m_state)
    {
    case INPUT_NORMAL_STATE:
            color = 1.5*GuiSystem::getInstance()->getNormalBackground();
        break;

    case INPUT_HOVER_STATE:
            color = 1.2*GuiSystem::getInstance()->getHoverBackground();
        break;

    case INPUT_SELECTED_STATE:
            color = GuiSystem::getInstance()->getHighlightBackground();
        break;
    }

    float size = m_labelText->getFont()->getTextureWith() / m_labelText->getSize();
    int maxChars = (int) ceil(3.7*(m_width / size));
    //MLOG_INFO("Max chars: " << maxChars);

    if(m_label.length() > maxChars)
        m_labelText->setText(m_label.substr(m_label.length() - maxChars).c_str());
    else
        m_labelText->setText(m_label.c_str());

    MRenderingContext* renderContext = NeoEngine::getInstance()->getRenderingContext();

    render->drawColoredQuad(m_x, m_y, m_width, m_height, color);

    renderContext->enableScissorTest();
    renderContext->setScissor(m_x, winh - (m_y+m_height), m_width, m_height);

    /*renderContext->setClearColor(MVector4(1,0,0, 1));
    renderContext->clear(M_BUFFER_COLOR);
    renderContext->setClearColor(MVector4(0,0,0,1));*/

    render->drawText(m_labelText, m_x, m_y + 0.5*m_labelText->getSize() + 0.5*m_height);
    renderContext->disableScissorTest();
}

void InputField::addCharacter(char c)
{
#ifndef ANDROID
    m_label.erase(--m_label.end(), m_label.end());
    m_label += c;
    m_label += "|";
#endif
}

void InputField::update()
{
#ifndef ANDROID
    MMouse* mouse = MMouse::getInstance();
    MKeyboard* kbd = MKeyboard::getInstance();

    unsigned int x = mouse->getXPosition();
    unsigned int y = mouse->getYPosition();

    if(x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y + m_height && m_state != INPUT_SELECTED_STATE)
    {
        m_state = INPUT_HOVER_STATE;
    }
    else if(m_state != INPUT_SELECTED_STATE)
    {
        m_state = INPUT_NORMAL_STATE;
    }
    else if(!(x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y + m_height) &&
            m_state == INPUT_SELECTED_STATE && mouse->isLeftButtonPushed())
    {
        m_label.erase(--m_label.end(), m_label.end());
        m_state = INPUT_NORMAL_STATE;
    }

    if(m_state == INPUT_HOVER_STATE && mouse->isLeftButtonPushed())
    {
        m_label += "|";
        m_state = INPUT_SELECTED_STATE;
    }

    if(m_state == INPUT_SELECTED_STATE)
    {
        bool shift = false;

        if(kbd->isKeyPressed(MKEY_LSHIFT) || kbd->isKeyPressed(MKEY_RSHIFT))
        {
            shift = true;
        }

        for(int i = MKEY_A; i < MKEY_Z; i++)
        {
            if(kbd->isKeyPressed(i))
            {
                kbd->onKeyUp(i);

                if(!shift)
                    addCharacter(tolower(i));
                else
                    addCharacter(i);
            }
        }

        for(int i = MKEY_0; i <= MKEY_9; i++)
        {
            if(kbd->isKeyPressed(i))
            {
                kbd->onKeyUp(i);

                if(!shift)
                    addCharacter(i);
                else
                    addCharacter(i-16);
            }
        }

        if(kbd->isKeyPressed(MKEY_SPACE))
        {
            kbd->onKeyUp(MKEY_SPACE);
            addCharacter(' ');
        }

        if(kbd->isKeyPressed(MKEY_BACKSPACE))
        {
            kbd->onKeyUp(MKEY_BACKSPACE);
            if(m_label.length() >= 2)
            m_label.erase(m_label.end() - 2, --m_label.end());
        }
    }
#endif
}

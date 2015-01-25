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

#include <Sprite.h>
#include <Render.h>
#include <GuiSystem.h>

using namespace Neo::Gui;

void Sprite::update()
{

}

void Sprite::draw()
{
    Render* render = Render::getInstance();
    GuiSystem* gui = GuiSystem::getInstance();
    MSystemContext* system = NeoEngine::getInstance()->getSystemContext();
    Level* level = NeoEngine::getInstance()->getLevel();
	OCamera * camera = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getCurrentCamera();

    if(m_labelText == NULL)
    {
        m_labelText = render->createText(gui->getDefaultFont(), gui->getDefaultFontSize());
        m_labelText->setAlign(TEXT_ALIGN_CENTER);
    }

    if(m_image == 0 && m_imagePath.length() > 0)
    {
        char buf[256];
        getGlobalFilename(buf, system->getWorkingDirectory(), m_imagePath.c_str());
        m_image = level->loadTexture(buf)->getTextureId();
    }
	if (_isVisible){
		if (!_ignor_camera_offset){ 
			MVector3 m = camera->getPosition();
			float x = -m.x + gui->_camera_offset.x;
			float y = m.y + gui->_camera_offset.y;
			render->drawTexturedQuad(m_x + x, m_y + y, m_width, m_height, m_image, m_rotation, _scale, _flip);

			if (m_label.length() > 0)
			{
				m_labelText->setText(m_label.c_str());
				render->drawText(m_labelText, x + m_x + 0.5*m_width, y + m_y + m_height);
			}
		}else{
			render->drawTexturedQuad(m_x, m_y, m_width, m_height, m_image, m_rotation, _scale, _flip);
			if (m_label.length() > 0)
			{
				m_labelText->setText(m_label.c_str());
				render->drawText(m_labelText, m_x + 0.5*m_width, m_y + m_height);
			}
		}
	}
}

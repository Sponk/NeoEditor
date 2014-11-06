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

#include <Canvas.h>
#include <Render.h>

using namespace Neo;

void Canvas::draw()
{
    Render* render = Render::getInstance();
    MSystemContext* system = MEngine::getInstance()->getSystemContext();
    system->getScreenSize(&m_width, &m_height);

    // Clear the canvas
    render->drawColoredQuad(0,0, m_width, m_height, m_clearColor);

    // Draw all widgets
    for(int i = 0; i < m_widgets.size(); i++)
    {
        m_widgets[i]->draw();
    }
}

void Canvas::update()
{
    // Update all widgets
    for(int i = 0; i < m_widgets.size(); i++)
    {
        m_widgets[i]->update();
    }
}

void Canvas::addWidget(Widget* w)
{
    if(w)
        m_widgets.push_back(w);
}

void Canvas::clear()
{
    m_widgets.clear();
}

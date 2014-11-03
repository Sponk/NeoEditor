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
#ifndef __CANVAS_H_
#define __CANVAS_H_

#include <Widget.h>
#include <vector>
#include <MEngine.h>

namespace Neo
{
/**
 * @brief The Canvas class contains all widgets and renders them.
 *
 * A canvas contains all widgets, layouts, groups.
 * All elements can be rendered directly to the screen or to a texture.
 */
class Canvas
{
private:
    std::vector<Widget*> m_widgets;
    MVector4 m_clearColor;

    unsigned int m_width, m_height;

public:
    static Canvas* getInstance() { static Canvas m_instance; return &m_instance; }

    void draw();
    void update();

    void addWidget(Widget* w);
    MVector4 getClearColor() { return m_clearColor; }
    void setClearColor(MVector4 vec) { m_clearColor = vec; }
};
}

#endif

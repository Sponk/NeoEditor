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

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <string>
#include <MCore.h>

namespace Neo
{

#ifndef __MINGW32__
typedef void (*CALLBACK_FUNCTION)(long int);
#else
#define CALLBACK_FUNCTION void*
#endif

/**
 * @brief The Widget class contains all information that is common
 * to all GUI widgets.
 *
 * Every object that resembles a GUI widget has to derive from this class.
 */
class Widget
{
protected:
    float m_x, m_y;
    unsigned int m_width, m_height;
    std::string m_label;

    CALLBACK_FUNCTION m_callback;
    long int m_userData;

public:

    Widget(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char* label);
    Widget();

    virtual void draw() = 0;
    virtual void update() = 0;

    void setCallback(CALLBACK_FUNCTION func) { m_callback = func; }
    void setCallback(CALLBACK_FUNCTION func, long int data) { m_callback = func; m_userData = data; }

    long int getUserData() { return m_userData; }
    void setUserData(long int data) { m_userData = data; }

    const char* getLabel() { return m_label.c_str(); }
    void setLabel(const char* l) { m_label = l; }

    void setPosition(MVector2 pos) { m_x = pos.x; m_y = pos.y; }
    MVector2 getPosition() { return MVector2(m_x, m_y); }

    void doCallback();
};
}

#endif

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
namespace Gui
{

#ifndef WIN32
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
    float m_rotation;
    unsigned int m_width, m_height;
    std::string m_label;
    bool _isVisible = true;
    MVector2 _scale = MVector2(1, 1);
    MVector2 _flip = MVector2(1, 1);

    CALLBACK_FUNCTION m_callback;
    long int m_userData;

public:

    Widget(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char* label);
    Widget();

	/**
	 * @brief Virtual function used to draw the widget.
	 */
    virtual void draw() = 0;

	/**
	 * @brief Virtual function used to update the widget.
	 */
    virtual void update() = 0;
/*
	*Should this Widget be visible?
	*/
	void setVisible(bool value) { _isVisible = value; }
	/*
	*Scale the Widget
	*/
	void setScale(MVector2 scale){ _scale = scale; }
	/*
	*Flip the Widget
	*/
	void setFlip(MVector2 flip){ _flip = flip; }
	/**
	 * @brief Sets the callback.
	 *
	 * A callback is a function pointer of the type (*CALLBACK_FUNCTION)(long int) and
	 * points to a procedure of the form:
	 *
	 * /code
	 * void someCallback(long int) {}
	 * /endcode
	 *
	 * @param func The function pointer
	 */
    void setCallback(CALLBACK_FUNCTION func) { m_callback = func; }

	/**
	 * @brief Sets the callback and appends user data to it.
	 *
	 * The userdata will be the parameter of your custom callback procedure
	 * when it gets called.
	 *
	 * @see Widget::setCallback
	 *
	 * @param func The function pointer
	 * @param data The user data
	 */
	void setCallback(CALLBACK_FUNCTION func, long int data) { m_callback = func; m_userData = data; }

	/**
	 * @brief Retrieves the user data that will be given to every callback call.
	 * @return The user data.
	 */
    long int getUserData() { return m_userData; }

	/**
	 * @brief Changes the user data that will be given to every callback call.
	 * @param data The new user data.
	 */
    void setUserData(long int data) { m_userData = data; }

	/**
	 * @brief Gets the currently displayed label a C string.
	 * @return The current label.
	 */
    const char* getLabel() { return m_label.c_str(); }

	/**
	 * @brief Changes the current label of the widget.
	 * @param l The new label.
	 */
	void setLabel(const char* l) { m_label = l; }

	/**
	 * @brief Changes the widget position.
	 * @param pos The new position.
	 */
    void setPosition(MVector2 pos) { m_x = pos.x; m_y = pos.y; }

	/**
	 * @brief Retrieves the current position.
	 * @return The current position.
	 */
    MVector2 getPosition() { return MVector2(m_x, m_y); }

	/**
	 * @brief Changes the widget rotation.
	 * @param rot The new rotation.
	 */
    void setRotation(float rot) { m_rotation = rot; }

	/**
	 * @brief Retrieves the current rotation.
	 * @return The current rotation.
	 */
    float getRotation() { return m_rotation; }

	/**
	 * @brief Calls the callback with the user data as an argument.
	 */
    void doCallback();

	// For runtime identification
	const char* getStaticName() { return "Widget"; }
};
}
}
#endif

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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit
 *diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <NeoEngine.h>
#include <Neo2D.h>
#include <string>

namespace Neo
{
namespace Gui
{

// Predefinition
class Widget;

#if !defined(WIN32)
	/**
	 * A pointer to a callback.
	 */
	typedef void (*CALLBACK_FUNCTION)(Widget*, long int);
#elif  !defined(SWIG)
#define CALLBACK_FUNCTION void *
#endif

/**
 * @brief The Widget class contains all information that is common
 * to all GUI widgets.
 *
 * Every object that resembles a GUI widget has to derive from this class.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Widget
{
protected:
	/// The 2D coordinates of the widget
	float m_x, m_y;

	/// The rotation of the widget
	float m_rotation;

	/// The width and height of the widget
	unsigned int m_width, m_height;

	/// The label of the widget
	std::string m_label;

#ifndef SWIG
	/// The callback that should be called
	CALLBACK_FUNCTION m_callback;
#endif

	String m_callbackScript;

	/// Some userdata that is given to the callback
	long int m_userData;

	/// Is the widget visible?
	bool m_visible;

	/// The scale vector
	Vector2 m_scale;

	/// The flip vector
	Vector2 m_flip;
	float m_fontSize;
private:
	/**
	 * @brief Does nothing.
	 *
	 * Used to deliver the default callback to prevent crashes.
	 * @param data Some data.
	 */
	static void doNothing(Widget* w, long data) {}

	static void callScript(Widget* w, long data)
	{
		Neo::ScriptContext* script = Neo::NeoEngine::getInstance()->getScriptContext();
		script->callFunction(w->m_callbackScript.getSafeString());
	}

public:
	Widget(unsigned int x, unsigned int y, unsigned int width,
		   unsigned int height, const char* label);
	Widget();

	float getFontSize() { return m_fontSize; }
	void setFontSize(float s) { m_fontSize = s; }

	/**
	 * @brief Draws the widget to the canvas it belongs to.
	 * @see Canvas
	 * @see draw(Vector2 offset)
	 */
	virtual void draw() { draw(Vector2(0, 0)); }

	/**
	 * @brief Draws the widget to the canvas it belongs to.
	 * @param offset The offset to apply to the position.
	 * Used for emulating a camera.
	 *
	 * @see Canvas
	 */
	virtual void draw(Vector2 offset) = 0;

	/**
	 * @brief Updates the widget and calls the callback if necessary.
	 */
	virtual void update() = 0;

	/**
	 * @brief Sets the callback.
	 *
	 * A callback is a function pointer of the type (*CALLBACK_FUNCTION)(long
	 *int) and
	 * points to a procedure of the form:
	 *
	 * /code
	 * void someCallback(long int) {}
	 * /endcode
	 *
	 * @param func The function pointer
	 */
#ifndef SWIG
	void setCallback(CALLBACK_FUNCTION func) { m_callback = func; }
#endif

	void setScriptCallback(const char* name)
	{
		m_callbackScript = name;
		m_callback = (CALLBACK_FUNCTION) Widget::callScript;
	}

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
#ifndef SWIG
	void setCallback(CALLBACK_FUNCTION func, long int data)
	{
		m_callback = func;
		m_userData = data;
	}
#endif
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
	void setPosition(Vector2 pos)
	{
		m_x = pos.x;
		m_y = pos.y;
	}

	/**
	 * @brief Retrieves the current position.
	 * @return The current position.
	 */
	Vector2 getPosition() { return Vector2(m_x, m_y); }

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
	 * @brief Translates the object.
	 * @param vec The offset to apply.
	 */
	void translate(Vector2 vec)
	{
		m_x += vec.x;
		m_y += vec.y;
	}

	/**
	 * @brief Rotates the object.
	 * @param value The offset to apply.
	 */
	void rotate(float value) { m_rotation += value; }

	/**
	 * @brief Calls the callback with the user data as an argument.
	 */
	void doCallback();

	/**
	 * @brief Returns if the widget is turned visible.
	 * @return The boolean value.
	 */
	bool isVisible() { return m_visible; }

	/**
	 * @brief Sets the visibility status
	 * @param v The new status
	 */
	void setVisible(bool v) { m_visible = v; }
	/**
	 * @brief setScale Scale this Widget
	 * @param scale The ammount you want to scale with
	 */
	void setScale(Vector2 scale) { m_scale = scale; }
	Vector2 getScale() { return m_scale; }
	/**
	 * @brief setFlip Flip this Widget
	 */
	void setFlip(Vector2 flip) { m_flip = flip; }
	Vector2 getFlip() { return m_flip; }

	// For runtime identification
	const char* getStaticName() { return "Widget"; }
};
}
}
#endif

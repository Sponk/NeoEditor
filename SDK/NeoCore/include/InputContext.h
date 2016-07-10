//========================================================================
// Copyright (c) 2016 Yannick Pflanzer <neo-engine.de>
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

#ifndef __INPUT_CONTEXT
#define __INPUT_CONTEXT

#include <memory>
#include <vector>

namespace Neo
{

/**
 * @brief Definitions for key values.
 */
enum INPUT_KEYS
{
	KEY_SPACE = 32,
	KEY_0 = 48,
	KEY_1 = 49,
	KEY_2 = 50,
	KEY_3 = 51,
	KEY_4 = 52,
	KEY_5 = 53,
	KEY_6 = 54,
	KEY_7 = 55,
	KEY_8 = 56,
	KEY_9 = 57,
	KEY_A = 65,
	KEY_B = 66,
	KEY_C = 67,
	KEY_D = 68,
	KEY_E = 69,
	KEY_F = 70,
	KEY_G = 71,
	KEY_H = 72,
	KEY_I = 73,
	KEY_J = 74,
	KEY_K = 75,
	KEY_L = 76,
	KEY_M = 77,
	KEY_N = 78,
	KEY_O = 79,
	KEY_P = 80,
	KEY_Q = 81,
	KEY_R = 82,
	KEY_S = 83,
	KEY_T = 84,
	KEY_U = 85,
	KEY_V = 86,
	KEY_W = 87,
	KEY_X = 88,
	KEY_Y = 89,
	KEY_Z = 90,

	KEY_BACKSPACE = 128,
	KEY_TAB = 129,
	KEY_CLEAR = 130,
	KEY_RETURN = 131,
	KEY_PAUSE = 132,
	KEY_ESCAPE = 133,
	KEY_DELETE = 134,
	KEY_KP0 = 135,
	KEY_KP1 = 136,
	KEY_KP2 = 137,
	KEY_KP3 = 138,
	KEY_KP4 = 139,
	KEY_KP5 = 140,
	KEY_KP6 = 141,
	KEY_KP7 = 142,
	KEY_KP8 = 143,
	KEY_KP9 = 144,
	KEY_KP_ENTER = 145,
	KEY_UP_ARROW = 146,
	KEY_DOWN_ARROW = 147,
	KEY_RIGHT_ARROW = 148,
	KEY_LEFT_ARROW = 149,
	KEY_INSERT = 150,
	KEY_HOME = 151,
	KEY_END = 152,
	KEY_PAGEUP = 153,
	KEY_PAGEDOWN = 154,
	KEY_F1 = 155,
	KEY_F2 = 156,
	KEY_F3 = 157,
	KEY_F4 = 158,
	KEY_F5 = 159,
	KEY_F6 = 160,
	KEY_F7 = 161,
	KEY_F8 = 162,
	KEY_F9 = 163,
	KEY_F10 = 164,
	KEY_F11 = 165,
	KEY_F12 = 166,
	KEY_NUMLOCK = 167,
	KEY_RSHIFT = 168,
	KEY_LSHIFT = 169,
	KEY_RCONTROL = 171,
	KEY_LCONTROL = 172,
	KEY_RALT = 174,
	KEY_LALT = 175,
	KEY_ADD = 176,
	KEY_SUB = 177,
	KEY_HELP = 178,
	KEY_PRINT = 179,
	KEY_MENU = 180,
	KEY_RSUPER = 181,
	KEY_LSUPER = 182,

	WINDOW_SELECT = 254, // FIXME: Should this be a _key_?
	KEY_DUMMY = 255
};

enum INPUT_MOUSE_BUTTONS
{
	MOUSE_BUTTON_LEFT = 0x0,
	MOUSE_BUTTON_MIDDLE = 0x1,
	MOUSE_BUTTON_RIGHT = 0x2
};

/**
 * @brief All possible states a key can have.
 */
enum KEY_STATES
{
	KEY_UP = 0, /// The key is not pressed
	KEY_DOWN = 1, /// The key is pressed
	KEY_ON_UP = 2, /// The key was released in this frame
	KEY_ON_DOWN = 3 /// The key was pressed in this frame
};

class NEO_CORE_EXPORT InputDevice
{
private:
	std::vector<unsigned int> m_keys;

public:

	InputDevice() {}
	InputDevice(size_t keys)
	{
		getKeys().reserve(keys);
		for(int i = 0; i < keys; i++)
			m_keys.push_back(KEY_UP);
	}

	std::vector<unsigned int>& getKeys() { return m_keys; }

	void keyDown(unsigned int key) { if(!isKeyDown(key)) m_keys[key] = KEY_ON_DOWN; }
	void keyUp(unsigned int key) { if(isKeyDown(key)) m_keys[key] = KEY_ON_UP; }

	bool isKeyDown(unsigned int key) { return (m_keys[key] == KEY_DOWN || m_keys[key] == KEY_ON_DOWN); }
	bool onKeyDown(unsigned int key) { return m_keys[key] == KEY_ON_DOWN; }
	bool onKeyUp(unsigned int key) { return m_keys[key] == KEY_ON_UP; }

	virtual void flush()
	{
		for(int i = 0; i < m_keys.size(); i++)
			if(m_keys[i] > KEY_DOWN)
				m_keys[i] -= 2; // Shift it up two places.
	}
};

class NEO_CORE_EXPORT AxisInputDevice : public InputDevice
{
	std::vector<float> m_axis;
	
	void initAxis(unsigned int num)
	{
		m_axis.reserve(num);
		for(int i = 0; i < num; i++)
			m_axis.push_back(0.0f);
	}
public:
	AxisInputDevice(unsigned int numberOfAxis)
	{
		initAxis(numberOfAxis);
	}

	AxisInputDevice(unsigned int numberOfAxis, unsigned int numberOfKeys)
		: InputDevice(numberOfKeys)
	{
		initAxis(numberOfAxis);
	}

	float getAxis(unsigned int id) { return m_axis[id]; }
	void setAxis(unsigned int id, float val) { m_axis[id] = val; }
};

/**
 * @brief A keyboard that handles
 */
class NEO_CORE_EXPORT Keyboard : public InputDevice
{
	unsigned int m_character; // The last unicode character that was typed
public:
	// Reserve enough memory to hold all keys!
	Keyboard() : InputDevice(KEY_DUMMY + 1), m_character(EOF) {}

	unsigned int getCharacter() const { return m_character; }
	void setCharacter(unsigned int c) { m_character = c; }
};

class NEO_CORE_EXPORT Mouse : public AxisInputDevice
{
private:
	Vector2 m_position;
	Vector2 m_oldPosition;
	Vector2 m_direction;
	float m_scrollValue;

public:
	Mouse() :
		m_position(0, 0),
		m_direction(0, 0),
		m_oldPosition(0, 0),
		m_scrollValue(0),
		AxisInputDevice(2, 3) {}

	const Vector2& getPosition() const
	{
		return m_position;
	}

	void moveCursor(const Vector2& position)
	{
		//m_direction = position - m_position;
		m_position = position;
	}

	const Vector2& getDirection() const
	{
		return m_direction;
	}

	void setDirection(const Vector2& v)
	{
		m_direction = v;
	}

	float getScrollValue() const
	{
		return m_scrollValue;
	}

	void setScrollValue(float scrollValue)
	{
		m_scrollValue = scrollValue;
	}

	virtual void flush()
	{
		InputDevice::flush();
	}

	void flushDirection()
	{
		m_direction = m_position - m_oldPosition;
		m_oldPosition = m_position;
	}
};

class NEO_CORE_EXPORT InputContext
{
private:
	Keyboard m_keyboard; /// Contains the overall state of the keyboard
	Mouse m_mouse;

protected:
#ifndef SWIG
	std::vector<std::shared_ptr<Neo::AxisInputDevice>> m_controllers;
#endif

public:
	Keyboard& getKeyboard() { return m_keyboard; }
	Mouse& getMouse() { return m_mouse; }

	const shared_ptr<Neo::AxisInputDevice>& getController(size_t id) const { return m_controllers[id]; }

	virtual void handleInput() = 0;
	bool isKeyDown(INPUT_KEYS key) { return m_keyboard.isKeyDown(key); }
	bool onKeyDown(INPUT_KEYS key) { return m_keyboard.onKeyDown(key); }
	bool onKeyUp(INPUT_KEYS key) { return m_keyboard.onKeyUp(key); }

	void flush()
	{
		m_keyboard.flush();
		m_mouse.flush();
	}
};

class NEO_CORE_EXPORT InputContextDummy : public InputContext
{
public:
	virtual void handleInput() {}
};

}
#endif

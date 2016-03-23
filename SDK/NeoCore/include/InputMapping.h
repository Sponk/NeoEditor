//========================================================================
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef __INPUT_MAPPING
#define __INPUT_MAPPING

#include <unordered_map>
#include <string>

namespace Neo
{
class NEO_CORE_EXPORT InputMapping
{
	InputContext& m_input;
	std::unordered_map<std::string, INPUT_KEYS> m_keymap;
public:
	InputMapping(InputContext& input)
		: m_input(input) {}

	virtual ~InputMapping(void){}

	void mapKey(const char* name, INPUT_KEYS key) { m_keymap[name] = key; }
	void mapAxis(const char* name, unsigned int joystickId, unsigned int axis) {}

	bool isKeyDown(const char* name) { return m_input.isKeyDown(m_keymap[name]); }
	bool onKeyDown(const char* name) { return m_input.onKeyDown(m_keymap[name]); }
	bool onKeyUp(const char* name) { return m_input.onKeyUp(m_keymap[name]); }
};
}
#endif

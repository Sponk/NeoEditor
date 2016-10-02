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

#ifndef __CONFIGURATIONREGISTRY_H
#define __CONFIGURATIONREGISTRY_H

namespace Neo
{
/**
 * @brief Provides a registry of named configuration strings.
 *
 * Every variable registered might be provided with an "on change" callback to prevent
 * polling. If polling is desired the pointer returned by the register function can be used.
 */
class NEO_ENGINE_EXPORT ConfigurationRegistry
{
	struct Variable
	{
		std::string str;
		std::function<void(std::string&)> callback = nullptr;
	};

	std::unordered_map<std::string, Variable> m_registry;
public:
	std::string* registerVariable(const std::string& name, std::function<void(std::string&)> cb = nullptr);
	void removeVariable(const std::string& name);
	void setVariable(const std::string& name, const std::string& value);
	std::string* getVariable(const std::string& name) { return &m_registry[name].str; }
	bool exists(const std::string& name) { return m_registry.end() != m_registry.find(name); }
		
	bool load(const std::string& path);
	bool save(const std::string& path);
};
}

#endif

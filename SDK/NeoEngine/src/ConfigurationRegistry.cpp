#include <NeoEngine.h>

using namespace Neo;

std::string& ConfigurationRegistry::registerVariable(const std::string& name, std::function<void(std::string&)> cb)
{
	Variable& v = m_registry[name];
	v.callback = cb;
	return v.str;
}

void ConfigurationRegistry::setVariable(const std::string& name, const std::string& value)
{
	auto i = m_registry.find(name);
	if(i != m_registry.end())
	{
		i->second.str = value;

		if (i->second.callback)
			i->second.callback(i->second.str);
	}
}



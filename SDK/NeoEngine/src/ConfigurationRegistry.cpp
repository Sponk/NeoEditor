#include <NeoEngine.h>
#include <sexpresso.hpp>

using namespace Neo;

std::string* ConfigurationRegistry::registerVariable(const std::string& name, std::function<void(std::string&)> cb)
{
	Variable& v = m_registry[name];
	v.callback = cb;
	return &v.str;
}

void ConfigurationRegistry::removeVariable(const std::string& name)
{
	m_registry.erase(name);
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

bool ConfigurationRegistry::load(const std::string& path)
{
	char* file = readTextFile(path.c_str());
	if(file == nullptr)
		return false;
	
	auto lispConfig = sexpresso::parse(file);
	auto editor = lispConfig.getChildByPath("config");

	if(!editor)
		return false;

	for (auto p : editor->arguments())
		*getVariable(p.value.sexp[0].value.str) = p.value.sexp[1].value.str;

	delete file;
	return true;
}

bool ConfigurationRegistry::save(const std::string& path)
{
	std::ofstream out(path);
	if(!out) return false;
	
	out << "(config" << std::endl;
	for(auto& p : m_registry)
	{
		out << "\t(" << p.first << " \"" << p.second.str << "\")" << std::endl;
	}
	out << ")" << std::endl;
	out.close();
	
	return true;
}

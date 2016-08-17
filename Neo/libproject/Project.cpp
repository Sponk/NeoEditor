#include "Project.h"
#include <fstream>
#include <vector>
#include <sexpresso.hpp>
#include <NeoCore.h>

bool Project::load(const char* path)
{
	char* content = readTextFile(path);
	if(!content) return false;

	auto project = sexpresso::parse(content);
	SAFE_DELETE(content);

	auto startlevel = project.getChildByPath("project/start-level");
	auto name = project.getChildByPath("project/name");
	auto author = project.getChildByPath("project/author");
	auto version = project.getChildByPath("project/version");
	auto neoversion = project.getChildByPath("project/neo-version");
	auto assetdir = project.getChildByPath("project/assets");
	auto description = project.getChildByPath("project/description");
	auto plugins = project.getChildByPath("project/plugins");
	auto additionalFiles = project.getChildByPath("project/additional-files");

	if(!startlevel || !name || !author || !version || !neoversion || !assetdir || !description || !plugins)
		return false;

	m_startLevel = startlevel->arguments().begin()->value.str;
	m_name = name->arguments().begin()->value.str;
	m_version = version->arguments().begin()->value.str;
	m_author = author->arguments().begin()->value.str;
	m_neoVersion = neoversion->arguments().begin()->value.str;
	m_assets = assetdir->arguments().begin()->value.str;
	m_description = description->arguments().begin()->value.str;

	for(auto p : plugins->arguments())
	  m_plugins.push_back(p.value.str);
	
	if(additionalFiles)
	{
		for (auto p : additionalFiles->arguments())
			m_additionalFiles.push_back(p.value.str);
	}

	m_filepath = path;
	return true;
}

bool Project::save(const char* path)
{
	std::ofstream out(path);
	if(!out) return false;

	out << "(project " << std::endl; 
	out << "\t(name \"" << m_name << "\")" << std::endl;
	out << "\t(author \"" << m_author << "\")" << std::endl;
	out << "\t(version \"" << m_version << "\")" << std::endl;
	out << "\t(neo-version \"" << m_neoVersion << "\")" << std::endl;
	out << "\t(start-level \"" << m_startLevel << "\")" << std::endl;
	out << "\t(assets \"" << m_assets << "\")" << std::endl;
	out << "\t(description \"" << m_description << "\")" << std::endl;
	
	out << "\t(plugins " << std::endl;
	for(auto p : m_plugins)
	  out << "\t\"" << p << "\"\n";
	out << "\t)" << std::endl;
	
	out << "\t(additional-files " << std::endl;
	for (auto p : m_additionalFiles)
		out << "\t\"" << p << "\"\n";
	out << "\t)" << std::endl;

	out << ")" << std::endl;

	m_filepath = path;
	out.close();
	return true;
}

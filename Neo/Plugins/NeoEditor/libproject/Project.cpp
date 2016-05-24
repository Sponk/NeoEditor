#include "Project.h"
#include <fstream>
#include <vector>
#include <sexpresso.hpp>
#include <NeoCore.h>

bool Project::load(const char* path)
{
	auto project = sexpresso::parse(readTextFile(path));
	auto startlevel = project.getChildByPath("project/start-level");
	auto name = project.getChildByPath("project/name");
	auto author = project.getChildByPath("project/author");
	auto version = project.getChildByPath("project/version");
	auto neoversion = project.getChildByPath("project/neo-version");
	
	if(!startlevel || !name || !author || !version || !neoversion)
		return false;

	m_startLevel = startlevel->arguments().begin()->value.str;
	m_name = name->arguments().begin()->value.str;
	m_version = version->arguments().begin()->value.str;
	m_author = author->arguments().begin()->value.str;
	m_neoVersion = neoversion->arguments().begin()->value.str;

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
	out << ")" << std::endl;
	
	out.close();
	return true;
}

#include "ProjectBackend.h"
#include <fstream>
#include <sexpresso.hpp>
#include <NeoCore.h>

bool ProjectBackend::createProject(const char* name, const char* path, const char* templ)
{
	auto iterator = m_templates.find(templ);
	if(iterator == m_templates.end())
        return false;

	Project ptempl = iterator->second;
	char src[256]; // FIXME: Make dynamic!
	getRepertory(src, ptempl.getFilePath().c_str());

	std::string fullpath = path;
	copyDirectory(src, path);

	Project newProject(name, "", "0.0", "Me", ptempl.getLevel().c_str(),
					   ptempl.getAssetDirectory().c_str(), ptempl.getNeoVersion().c_str());

    bool result = newProject.save((fullpath + "/" + name + ".nproj").c_str());
    m_projects.push_back(newProject);
    return result;
}

bool ProjectBackend::removeProject(size_t id, bool deleteFiles)
{
	if(id >= m_projects.size())
		return false;

	const Project& p = m_projects[id];
	if(deleteFiles)
	{
		remove(p.getFilePath().c_str());
		removeDirectory(p.getAssetDirectory().c_str());
	}

	m_projects.erase(m_projects.begin() + id);
	return true;
}

size_t ProjectBackend::importProject(const char* path)
{
	Project p;
	if(!p.load(path))
		return -1;

	m_projects.push_back(p);
	return m_projects.size() - 1;
}

std::string ProjectBackend::importTemplate(const char* path)
{
	Project p;
	if(!p.load(path)) return "";

	m_templates[p.getName()] = p;
	return p.getName();
}

bool ProjectBackend::saveConfig(const char* path)
{
	std::ofstream out(path);
	if(!out)
		return false;

	out << "(templates " << std::endl;

	for(auto v : m_templates)
		out << "\t\"" << v.second.getFilePath() << "\"" << std::endl;

	out << ")" << std::endl;

	out << "(projects " << std::endl;

	for(auto v : m_projects)
		out << "\t\"" << v.getFilePath() << "\"" << std::endl;
	out << ")" << std::endl;
	out.close();
	return true;
}

bool ProjectBackend::loadConfig(const char* path)
{
	char* content = readTextFile(path);
	if(!content)
		return false;

	auto root = sexpresso::parse(content);
	SAFE_DELETE(content);

	// Load templates
	{
		auto templates = root.getChildByPath("templates");
		if (!templates)
			return false;

		for (auto t : templates->arguments())
			importTemplate(t.value.str.c_str());
	}

	// Load projects
	{
		auto projects = root.getChildByPath("projects");
		if (!projects)
			return false;

		for (auto t : projects->arguments())
			importProject(t.value.str.c_str());
	}

	return true;
}

ProjectBackend::ProjectBackend(const std::string& config)
	: m_configDirectory(std::move(config))
{}

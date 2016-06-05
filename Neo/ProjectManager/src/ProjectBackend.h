#ifndef NEO_PROJECTBACKEND_H
#define NEO_PROJECTBACKEND_H

#include "../../libproject/Project.h"
#include <map>
#include <vector>
#include <string>

class ProjectBackend
{
	/// Maps template name to the respecting project
	std::map<std::string, Project> m_templates;

	/// Contains all registered projects
	std::vector<Project> m_projects;
	std::string m_configDirectory;

public:
	ProjectBackend(const std::string& config);
	ProjectBackend() {}

	void createProject(const char* name, const char* path, const char* templ);
	void removeProject(size_t id, bool deleteFiles = false);
	size_t importProject(const char* path);
	std::string importTemplate(const char* path);

	void saveConfig(const char* path);
	void loadConfig(const char* path);

	std::vector<Project>& getProjects()
	{
		return m_projects;
	}

	std::map<std::string, Project>& getTemplates()
	{
		return m_templates;
	}
};

#endif //NEO_PROJECTBACKEND_H

#ifndef __PROJECT_H
#define __PROJECT_H

#include <string>

class Project
{
	std::string m_name;
	std::string m_version;
	std::string m_author;
	std::string m_startLevel;
	std::string m_neoVersion;
public:

	Project() {}
	Project(const char* name, const char* version, const char* author,
			const char* start, const char* neoversion) :
		m_name(name),
		m_version(version),
		m_author(author),
		m_startLevel(start),
		m_neoVersion(neoversion)
	{}

	bool save(const char* path);
	bool load(const char* path);
	const std::string& getName() const { return m_name; }
	const std::string& getVersion() const { return m_version; }
	const std::string& getAuthor() const { return m_author; }
	const std::string& getLevel() const { return m_startLevel; }
	const std::string& getNeoVersion() const { return m_neoVersion; }
};

#endif

#ifndef __PROJECT_H
#define __PROJECT_H

#include <string>
#include <vector>

/**
 * @brief Represents an abstract game project.
 *
 * This class encapsules all data needed to load, save and process a game project.
 */
class Project
{
	std::string m_name;
	std::string m_version;
	std::string m_author;
	std::string m_startLevel;
	std::string m_neoVersion;
	std::string m_filepath;
	std::string m_assets;
	std::string m_description;

	std::vector<std::string> m_plugins;
public:

	Project() {}
	Project(const char* name, const char* description, const char* version, const char* author,
			const char* start, const char* assets, const char* neoversion) :
		m_name(name),
		m_version(version),
		m_author(author),
		m_startLevel(start),
		m_neoVersion(neoversion),
		m_assets(assets),
		m_description(description)
	{}

	/**
	 * @brief Saves the project to a file.
	 *
	 * This will not save the level or any loaded assets.
	 * @param path The project file to save.
	 * @return A boolean value representing success.
	 */
	bool save(const char* path);

	/**
	 * @brief Loads the project from a file.
	 *
	 * This will not load the level or any assets.
	 * @param path The project file to load.
	 * @return A boolean value representing success.
	 */
	bool load(const char* path);
	
	const std::string& getName() const { return m_name; }
	const std::string& getVersion() const { return m_version; }
	const std::string& getAuthor() const { return m_author; }

	const std::string& getLevel() const { return m_startLevel; }
	void setLevel(const std::string& level) { m_startLevel = level; }
	
	const std::string& getNeoVersion() const { return m_neoVersion; }
	const std::string& getFilePath() const { return m_filepath; }
	const std::string& getAssetDirectory() const { return m_assets; }
	const std::string& getDescription() const { return m_description; }
	const std::vector<std::string>& getPlugins() const { return m_plugins; }
};

#endif

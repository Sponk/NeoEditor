#ifndef NEO_PUBLISHER_H
#define NEO_PUBLISHER_H

#include <functional>
#include <unordered_map>

namespace Publish
{

/**
 * @brief Implements a class that allows to publish a project of some sort.
 *
 * The output depends on the actual implementation and might be an NPK, a ZIP or something else.
 */
class Publisher
{
public:

	/**
	 * @brief Delivers the name of the publishing solution.
	 * @return A unique name.
	 */
	virtual const char* getName() { return "NONE"; }

	/**
	 * @brief Publishes the given project file.
	 * @param projectFile The path to a Neo project file.
	 * @param progressCallback (optional) A callback that is being called to update the current progress.
	 * @return Returns if the operation was successful.
	 */
	virtual bool publish(const char* projectFile, const char* executable, const char* output,
						 bool verbose = false, const char* levelOverride = nullptr,
						 std::function<void(int)> progressCallback = nullptr) = 0;
};


class PublisherRegistry
{
	static std::unordered_map<std::string, Publisher*> m_publishers;
public:
	PublisherRegistry(Publisher& p)
	{
		PublisherRegistry::m_publishers[p.getName()] = &p;
	}

	static const std::unordered_map<std::string, Publisher*>& getPublishers() { return m_publishers; }
};

}

#endif //NEO_PUBLISHER_H

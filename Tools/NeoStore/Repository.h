#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <string>

// Change this to 80
#define DEFAULT_PORT 8000

class Repository
{
public:
	struct Package
	{
		std::string name;
		std::string path;
		std::string description;
		std::string destination;
		std::string author;
		std::string license;
		std::string version;
		std::string server;
	};

private:
	std::vector<Package> m_packages;
	std::string m_server;

	Package getPackageInfo(const char* path);

public:
	bool hasPackage(const char* name);
	Package findPackage(const char* name);
	bool getPackageList(const char* server);
	bool getPackageList();
	void setServer(const char* server) { m_server = server; }
	const char* getServer() { return m_server.c_str(); }

	Package getPackage(int idx) { return m_packages[idx]; }
	int getPackageCount() { return m_packages.size(); }
};

#endif

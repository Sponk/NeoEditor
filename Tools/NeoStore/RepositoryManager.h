#ifndef REPOSITORY_MANAGER_H
#define REPOSITORY_MANAGER_H

#include "Repository.h"

#include <vector>
#include <string>

class RepositoryManager
{
private:
	std::vector<Repository> m_repositories;
	std::vector<std::string> m_installed;

	std::string getInstalledPackagesListFile();
	std::string getInstalledPackagesListDir();

public:

	static RepositoryManager* getInstance()
	{
		static RepositoryManager mgr;
		return &mgr;
	}

	void addRepository(Repository r);
	void updatePackageInformation();

	Repository::Package findPackage(const char* name);
	bool hasPackage(const char* name);
	void getPackageList(std::vector<Repository::Package>* vec);

	bool installPackage(Repository::Package p);
	bool installPackage(const char* name);

	bool removePackage(Repository::Package p);
	bool removePackage(const char* name);

	bool isInstalled(const char* name);
	bool isInstalled(Repository::Package p);

	void saveInstalledPackagesList();
};

#endif

#include "RepositoryManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <MCore.h>
#include "Downloader.h"
#include <zip.h>
#include <cstdio>

using namespace std;

void RepositoryManager::addRepository(Repository r)
{
	m_repositories.push_back(r);
}

std::string RepositoryManager::getInstalledPackagesListFile()
{
	return getInstalledPackagesListDir() + "installed.txt";
}

std::string RepositoryManager::getInstalledPackagesListDir()
{
	string confPath;

#ifndef WIN32
	confPath = getenv("HOME");
	confPath += "/.neo-store/";
#else
	confPath = getenv("APPDATA");
	confPath += "\\.neo-store\\";
#endif

	return confPath;
}

void RepositoryManager::updatePackageInformation()
{
	for(int i = 0; i < m_repositories.size(); i++)
	{
		cout << "Updating repository: " << m_repositories[i].getServer() << endl;
		m_repositories[i].getPackageList();
	}

	// This file needs to be the same as in the ProjectManager!
	ifstream in(getInstalledPackagesListFile().c_str());

	if(!in)
		return;

	string line;
	while(!in.eof())
	{
		in >> line;
		m_installed.push_back(line);
	}

	in.close();
}

bool RepositoryManager::hasPackage(const char* name)
{
	for(int i = 0; i < m_repositories.size(); i++)
	{
		if(m_repositories[i].hasPackage(name)) return true;
	}

	return false;
}

Repository::Package RepositoryManager::findPackage(const char* name)
{
	for(int i = 0; i < m_repositories.size(); i++)
	{
		if(m_repositories[i].hasPackage(name)) return m_repositories[i].findPackage(name);
	}

	return Repository::Package();
}


void RepositoryManager::getPackageList(std::vector<Repository::Package>* vec)
{
	for(int i = 0; i < m_repositories.size(); i++)
	{
		for(int j = 0; j < m_repositories[i].getPackageCount(); j++)
		{
			vec->push_back(m_repositories[i].getPackage(j));
		}
	}
}

bool RepositoryManager::installPackage(Repository::Package p)
{
	cout << "Installing package " << p.name << endl;

	string fullDest;
	string filename;
#ifndef WIN32
	int idx = p.destination.find("$HOME");
	if(idx != -1)
		fullDest = p.destination.replace(idx, strlen("$HOME"), getenv("HOME"))+"/";
#else
	replace(p.destination.begin(), p.destination.end(), '/', '\\');

	int idx = p.destination.find("$HOME");

	if(idx != -1)
		fullDest = p.destination.replace(idx, strlen("$HOME"), getenv("APPDATA"))+"\\";
#endif

	filename = p.path.substr(p.path.find_last_of('/'));

#ifdef WIN32
	filename.replace(filename.begin(), filename.begin()+1, "\\");
#endif

	fullDest += p.name;

	string unixDest = fullDest;
#ifdef WIN32
	replace(unixDest.begin(), unixDest.end(), '\\', '/');
#endif

	if(!isFileExist(fullDest.c_str()))
	{
		if (!createDirectory(unixDest.c_str(), true))
		{
			cout << "Could not create installation directory! (" << fullDest << ")" << endl;
			return false;
		}
	}

	cout << "Installing to " << fullDest << endl;
	cout << "From " << p.server << p.path << endl;

	bool success = downloadFileToFile(p.server.c_str(), p.path.c_str(), (fullDest + filename).c_str(), NULL, DEFAULT_PORT);

	if(!success)
	{
		cout << "Could not download " << fullDest << filename << endl;
		return false;
	}

#ifndef WIN32
	fullDest += "/";
#else
	fullDest += "\\";
#endif

	if(filename.find(".zip") != -1)
	{
		cout << "Decompressing zip..." << endl;

		int err = 0;
		zip* z = zip_open((fullDest+filename).c_str(), 0, &err);

		struct zip_stat st;
		zip_stat_init(&st);

		int files = zip_get_num_files(z);

		//cout << "Archive has " << files << " files." << endl;

		for(int i = 0; i < files; i++)
		{
			zip_stat_index(z, i, 0, &st);
			//cout << "Found file " << st.name << endl;

			if(st.size == 0)
			{
				//cout << "File is an directory!" << endl;
				if(!isFileExist((fullDest+st.name).c_str()))
					createDirectory((fullDest+st.name).c_str());

				continue;
			}

			// TODO: Do this in smaller chunks!
			char* data = (char*) malloc(st.size);
			zip_file* f = zip_fopen(z, st.name, 0);
			zip_fread(f, data, st.size);
			zip_fclose(f);

			FILE* file = fopen((fullDest+st.name).c_str(), "wb");
			if(file)
			{
				fwrite(data, st.size, 1, file);
				fclose(file);
			}

			free(data);
		}

		zip_close(z);
	}

	m_installed.push_back(p.name + "-" + p.version);

	if(!isFileExist(getInstalledPackagesListDir().c_str()))
		createDirectory(getInstalledPackagesListDir().c_str(), true);

	saveInstalledPackagesList();

	cout << "Done." << endl;
	return true;
}

bool RepositoryManager::installPackage(const char* name)
{
	Repository::Package p = findPackage(name);

	if(p.name.empty())
	{
		cout << "Could not install package " << name << "!" << endl;
		return false;
	}

	return installPackage(p);
}

bool RepositoryManager::removePackage(Repository::Package p)
{
	cout << "Removing package " << p.name << endl;

	string fullDest;
	string filename;
#ifndef WIN32

	int idx = p.destination.find("$HOME");
	if(idx != -1)
		fullDest = p.destination.replace(idx, strlen("$HOME"), getenv("HOME"))+"/";

	filename = p.path.substr(p.path.find_last_of('/'));
#else
	replace(p.destination.begin(), p.destination.end(), '/', '\\');

	int idx = p.destination.find("$HOME");
	if(idx != -1)
		fullDest = p.destination.replace(idx, strlen("$HOME"), getenv("APPDATA"))+"\\";

	filename = p.path.substr(p.path.find_last_of('/'));
#endif

	fullDest += p.name;
	bool success = removeDirectory(fullDest.c_str());

	for(int i = 0; i < m_installed.size(); i++)
	{
		// FIXME: Not good enough!
		if(m_installed[i].find(p.name) != -1)
			m_installed.erase(m_installed.begin()+i);
	}

	saveInstalledPackagesList();

	return success;
}

bool RepositoryManager::removePackage(const char* name)
{
	Repository::Package p = findPackage(name);

	if(p.name.empty())
	{
		cout << "Could not remove package " << name << "!" << endl;
		return false;
	}

	return removePackage(p);
}

bool RepositoryManager::isInstalled(const char* name)
{
	for(int i = 0; i < m_installed.size(); i++)
	{
		if(m_installed[i].find(name) == 0)
			return true;
	}

	return false;
}

bool RepositoryManager::isInstalled(Repository::Package p)
{
	return isInstalled(p.name.c_str());
}

void RepositoryManager::saveInstalledPackagesList()
{
	FILE* file = fopen(getInstalledPackagesListFile().c_str(), "w");

	if(!file)
	{
		cout << "Could not write config file!" << endl;
		return;
	}

	for(int i = 0; i < m_installed.size(); i++)
	{
		fprintf(file, "%s\n", m_installed[i].c_str());
	}

	fclose(file);
}

void RepositoryManager::clearPackageInformation()
{
	for(int i = 0; i < m_repositories.size(); i++)
	{
		m_repositories[i].clear();
	}

	m_installed.clear();
}

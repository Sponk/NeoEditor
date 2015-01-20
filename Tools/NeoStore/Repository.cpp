#include "Repository.h"
#include "Downloader.h"
#include <iostream>
#include <sstream>
#include "ini.h"

using namespace std;

bool Repository::getPackageList(const char* server)
{
	m_server = server;
	int error;
	string packageList = downloadFileToString(server, "/repo.txt", &error, DEFAULT_PORT);

	if(error >= 400)
	{
		cout << "Given repository is not valid!" << endl;
		return false;
	}

	stringstream ss(packageList);
	string line;

	Package p;
	while(!ss.eof())
	{
		getline(ss, line, '\n');

		if(line.empty())
			continue;

		p = getPackageInfo(line.c_str());

		//cout << "Got package: " << endl << "\tName: " << p.name << endl << "\tDescription: " << p.description << endl;
		//cout << "\tPath: " << p.path << endl;

		if(!p.name.empty())
			m_packages.push_back(p);
	}
}

bool Repository::getPackageList()
{
	return getPackageList(m_server.c_str());
}

Repository::Package Repository::getPackageInfo(const char* path)
{
	Package p;
	string packageIni = downloadFileToString(m_server.c_str(), path, NULL, DEFAULT_PORT);

	// This is no valid package description! (TODO: Better check!)
	if(packageIni.find("[package]") == -1)
		return p;

	try
	{
		stringstream ss(packageIni);
		INI::Parser parser(ss);

		p.name = parser.top()("package")["name"];
		p.description = downloadFileToString(m_server.c_str(), parser.top()("package")["description"].c_str(), NULL, DEFAULT_PORT);
		p.path = parser.top()("package")["path"];
		p.destination = parser.top()("package")["install-destination"];
		p.author = parser.top()("package")["author"];
		p.license = parser.top()("package")["license"];
		p.version = parser.top()("package")["version"];
		p.server = m_server;
		return p;
	}
	catch(runtime_error e)
	{
		cerr << "Could not parse package ini: " << e.what() << endl;
		return p;
	}
}

bool Repository::hasPackage(const char *name)
{
	for(int i = 0; i < m_packages.size(); i++)
	{
		if(m_packages[i].name == name)
			return true;
	}

	return false;
}

Repository::Package Repository::findPackage(const char *name)
{
	for(int i = 0; i < m_packages.size(); i++)
	{
		if(m_packages[i].name == name)
			return m_packages[i];
	}

	return Repository::Package();
}

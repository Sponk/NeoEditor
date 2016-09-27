#include <iostream>
#include <string>
#include "Publisher.h"

#ifndef VERSION_STRING
#define VERSION_STRING "v0.1"
#endif

using namespace std;

struct Settings
{
	string outputDirectory;
	string inputProject;
	string startLevel;
	string player;
	string generator;
	bool printHelp;
	bool listGenerators;
	bool verbose;
};

void parseOpt(Settings& s, int argc, char* argv[])
{
	if(argc <= 1)
	{
		s.printHelp = true;
		return;
	}

	s.printHelp = false;
	s.verbose = false;
	s.listGenerators = false;

#ifndef WIN32 // Default values
	s.outputDirectory = "./published";
	s.player = "./NeoPlayer2";
#else
	s.outputDirectory = ".\\published";
	s.player = ".\\NeoPlayer2.exe";
#endif

	string arg;
	string opt;
	for (int i = 1; i < argc; i++)
	{
		arg = argv[i];

		if (arg == "-i")
		{
			i++;
			s.inputProject = argv[i];
		}

		else if (arg == "-o")
		{
			i++;
			s.outputDirectory = argv[i];
		}

		else if (arg == "-s")
		{
			i++;
			s.startLevel = argv[i];
		}

		else if (arg == "-p")
		{
			i++;
			s.player = argv[i];
		}

		else if (arg == "-v")
		{
			s.verbose = true;
		}

		else if (arg == "-h")
		{
			s.printHelp = true;
			return;
		}

		else if (arg == "-g")
		{
			i++;
			s.generator = argv[i];
		}

		else if (arg == "-hg")
		{
			s.listGenerators = true;
			return;
		}

		else
		{
			cout << "Unknown option '" << arg << "'" << endl;
			s.printHelp = true;
			return;
		}
	}

	return;
}

void printHelp(const char* path)
{
	cout << "Usage: " << path << " -i <project> -o <output> -s <startlevel> -p <player>"
		 << endl;
	cout << "\t\t-i\tInput project file" << endl;
	cout << "\t\t-o\tOutput directory. Defaults to \"./published\"" << endl;
	cout << "\t\t-s\tStart level. Defaults to the one specified in the project "
		"file." << endl;
	cout << "\t\t-p\tThe executable player file." << endl;
	cout << "\t\t-g\tThe name of the package generator to use." << endl;
	cout << "\t\t-v\tMakes the tool verbose." << endl;
	cout << "\t\t-h\tPrint this help." << endl;
	cout << "\t\t-hg\tPrint all available package generators." << endl;
	cout << "This tool has puffin powers!" << endl;
}

int main(int argc, char* argv[])
{
	auto publishers = Publish::PublisherRegistry::getPublishers();
	Settings settings;
	parseOpt(settings, argc, argv);

	if(settings.printHelp)
	{
		printHelp(argv[0]);
		return 0;
	}

	if(settings.listGenerators)
	{
		cout << "Available package generators:" << endl;
		for(auto p : publishers)
			cout << "\t" << p.first << endl;

		return 0;
	}

	auto publisher = publishers[settings.generator];
	if(!publisher)
	{
		cout << "Specified package generator does not exist!" << endl;
		return 1;
	}

	return publisher->publish(settings.inputProject.c_str(), settings.player.c_str(),
							   settings.outputDirectory.c_str(), settings.verbose, settings.startLevel.c_str(),
							   [](int p){
								   cout << "Progress: " << p << "%" << endl;
							   });
}


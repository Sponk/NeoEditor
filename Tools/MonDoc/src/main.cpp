#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <algorithm>

#include "LuaSource.h"

#define VERSION_STRING "0.1"

#ifndef WIN32
#define SEPERATOR "/"
#else
#define SEPERATOR "\\"
#endif

using namespace std;

struct Settings
{
	vector<string> inputFiles;
	string indexFile;
	string outputDirectory;
	bool showHelp;
};

Settings parseCommandLine(int argc, char* argv[])
{
	Settings s;
	s.showHelp = false;
	s.outputDirectory = ".";

	if (argc <= 1)
	{
		cout << "Not enough arguments given!" << endl;
		s.showHelp = true;
		return s;
	}

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-o"))
		{
			s.outputDirectory = argv[++i];
		}
		else if (!strcmp("-i", argv[i]))
		{
			s.indexFile = argv[++i];
		}
		else if (!strcmp(argv[i], "-h"))
		{
			s.showHelp = true;
			return s;
		}
		else
		{
			s.inputFiles.push_back(argv[i]);
		}
	}

	return s;
}

void displayHelp()
{
	cout << "This program is a generator for generating html documentation" << endl;
	cout << "from Lua source files. The syntax is similar to doxygen." << endl;
	cout << endl;
	cout << "Usage: mondoc -o <output> <input files>" << endl;
	cout << "\t-i: The file used to produce the index.html" << endl;
	cout << "\t-o: Selects the output directory" << endl;
	cout << endl;
	cout << "\"Der Mond, das blasse Auge der Nacht.\" - Heinrich Heine" << endl; 
}

string getFilename(string filename)
{
	int idx = filename.find_last_of(SEPERATOR);
	if (idx != -1)
		return filename.substr(idx + 1);

	return filename;
}

string getPath(string filename)
{
	int idx = filename.find_last_of(SEPERATOR);
	if (idx != -1)
	{
		return filename.erase(idx + 1);
	}

	return filename;
}

void copyFile(string src, string dest)
{
	ifstream in;
	ofstream out;

	in.open(src.c_str(), ios::in);
	if (!in)
	{
		cout << "Can't open file!" << endl;
		return;
	}

	out.open(dest.c_str(), ios::out);
	if (!in)
	{
		cout << "Can't open file!" << endl;
		in.close();
		return;
	}

	out << in.rdbuf();
	in.close();
	out.close();
}

void writeHtml(string luaFilename, string outputDirectory, int idx, Settings& s, ofstream& searchOut, string outputFilename = "")
{

	string filename;
	string title;

	if(outputFilename.empty())
		title = getFilename(luaFilename);
	else
		title = outputFilename;
	
	ofstream out;
	out.open((outputDirectory + SEPERATOR + title + ".html").c_str(), ios::out);
	if (!out)
	{
		cout << "Could not write file!" << endl;
		return;
	}

	out << "<head>" << endl;
	out << "<title> Reference | " << title << "</title>" << endl;
	out << "</head>" << endl;

	// Start writing html stuff
	out << "<html>" << endl;
	out << "<script src=\"lunr.js\"></script>" << endl;
	out << "<script src=\"searchprovider.js\"></script>" << endl;
	out << "<script src=\"searchindex.js\"></script>" << endl;
	out << "<script src=\"highlight.pack.js\"></script>" << endl;
	out << "<script>hljs.initHighlightingOnLoad();</script>" << endl;

	out << "<body onload='processOnLoadSearch();'>" << endl;
	out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"/>"
		<< endl;

	out << "<link rel=\"stylesheet\" type=\"text/css\" "
		   "href=\"syntax-style.css\"/>" << endl;

	out << "<div class='function' id='searchresults'></div>" << endl;
	out << "<div class='sidepanel'>" << endl;

	// Write search field
	out << "<form>" << endl;
	out << "<input type=\"text\" name=\"searchedit\" "
		   "placeholder=\"Search...\">" << endl;
	out << "<input type=\"button\" name=\"\" value=\"Search\"";
	out << " onclick=\"submitSearch(this.form.searchedit.value); return "
		   "false;\">" << endl;
	out << "</form>" << endl;

	for (int j = 0; j < s.inputFiles.size(); j++)
	{
		filename = getFilename(s.inputFiles[j]);

		out << "<a class='filelink' href='" << filename;
		out << ".html'>" << filename << "</a><br>" << endl;
	}

	out << "</div>" << endl;

	string html = generateHtml(luaFilename.c_str());
	out << html << "</body></html>";

	std::replace(html.begin(), html.end(), '\"', '\'');
	std::replace(html.begin(), html.end(), '\n', ' ');

	searchOut << "index.add({ id: " << idx << ", title: '";
	searchOut << title << "', ";
	searchOut << "body: \"" << html << "\"});" << endl;

	searchOut << "sites.push({title: '" << title << "',";
	searchOut << "link: '" << title << ".html'});" << endl;

	out.close();
}

int main(int argc, char* argv[])
{
	cout << "MonDoc v" << VERSION_STRING << endl;
	Settings s = parseCommandLine(argc, argv);
	string content;

	if (s.showHelp)
	{
		displayHelp();
		return 0;
	}

	cout << "Copying JS files" << endl;
	string execPath = getPath(argv[0]);
	copyFile(execPath + "style.css",
			 s.outputDirectory + SEPERATOR + "style.css");
	
	copyFile(execPath + "syntax-style.css",
			 s.outputDirectory + SEPERATOR + "syntax-style.css");
	
	copyFile(execPath + "searchprovider.js",
			 s.outputDirectory + SEPERATOR + "searchprovider.js");
	copyFile(execPath + "lunr.js", s.outputDirectory + SEPERATOR + "lunr.js");
	copyFile(execPath + "highlight.pack.js", s.outputDirectory + SEPERATOR + "highlight.pack.js");
	
	cout << "Processing " << s.inputFiles.size() << " file(s)" << endl;

	ofstream searchOut;
	searchOut.open((s.outputDirectory + SEPERATOR + "searchindex.js").c_str(),
				   ios::out);
	if (!searchOut)
	{
		cout << "Could not write search index!" << endl;
		cout << (s.outputDirectory + SEPERATOR + "searchindex.js") << endl;
		return 1;
	}

	// Write search index header

	searchOut << "var index = lunr(function () {this.field('title', {boost: "
				 "10}), this.field('body'), this.ref('id')});" << endl;

	searchOut << "var sites = Array();" << endl;

	if (!s.indexFile.empty())
	{
		writeHtml(s.indexFile, s.outputDirectory, 0, s, searchOut, "index");
	}

	for (int i = 0; i < s.inputFiles.size(); i++)
	{
		writeHtml(s.inputFiles[i], s.outputDirectory, i, s, searchOut);
	}

	return 0;
}

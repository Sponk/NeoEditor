#include "LuaSource.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

using namespace std;

struct Parameter
{
	string name;
	string description;
};

struct Comment
{
	string description;
	vector<Parameter> parameter;
	string returnDescription;
	string functionName;
};

void trimString(string &str, char c)
{
	string::size_type pos = str.find_last_not_of(c);

	if (pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(c);
		if (pos != string::npos)
			str.erase(0, pos);
	}
	else
		str.erase(str.begin(), str.end());
}

void trimWhitespace(string &str)
{
	trimString(str, ' ');
	trimString(str, '\t');
}

void replaceString(string &src, const char* s1, const char* s2)
{
	size_t s1Size = strlen(s1);
	size_t s2Size = strlen(s2);

	if(s1Size == 0)
        return;

	size_t idx = 0;
    while((idx = src.find(s1, idx)) != -1)
	{
        src.replace(idx, s1Size, s2);
        idx += s2Size;
    }
}


void replaceString(string &src, string &s1, string &s2)
{
	replaceString(src, s1.c_str(), s2.c_str());
}

string commentToString(Comment c)
{
	string output;

	output += "<div class='function'>";

	if (!c.functionName.empty())
	{
		output += "<h2>";
		output += c.functionName + "</h2><p>\n";
	}
	else
	{
		output += "<h2>";
		int idx = c.description.find("\n");

		string title = c.description;
		if (idx != -1)
		{
			title.erase(idx);
			output += title + "</h2><p>\n\n";

			c.description = c.description.substr(idx);
		}
	}

	replaceString(c.description, "/code", "<pre class='code'><code>");
	replaceString(c.description, "/endcode", "</code></pre>");
	replaceString(c.description, "\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
	replaceString(c.description, "    ", "&nbsp;&nbsp;&nbsp;&nbsp;");

	output += c.description + "</p>";

	int numParams = c.parameter.size();

	if (numParams > 0)
		output += "<p><h3>Parameter</h3>";

	Parameter p;
	for (int i = 0; i < numParams; i++)
	{
		p = c.parameter[i];
		output += "<p><strong>" + p.name + "</strong>\n";
		output += "<div class='indented'>" + p.description + "</div></p>\n";
	}

	if (numParams > 0)
		output += "</p>";

	if (!c.returnDescription.empty())
		output += "<p> <strong>Return:</strong> " + c.returnDescription + "</p>";

	output += "</div>";
	return output;
}

string generateHtml(const char *filename)
{
	cout << "Loading file " << filename << endl;
	ifstream in(filename);
	string output;

	if (!in)
	{
		cout << "Could not open file!" << endl;
		return "";
	}

	string line;
	bool insideCodeBlock = false;
	while (!in.eof())
	{
		std::getline(in, line);
		trimWhitespace(line);

		// We found a function description!
		if (line.find("---") == 0)
		{
			Comment comment;

			do
			{
				// Are we inside a code example?
				if(line.find("/code") != -1)
					insideCodeBlock = true;

				if(line.find("/endcode") != -1)
					insideCodeBlock = false;
				
				trimString(line, '-');

				// Only trim whitespaces if we don't need them!
				if(!insideCodeBlock)
					trimWhitespace(line);
			    				
				if (line.find("@param") == 0)
				{
					Parameter p;
					p.name = line.substr(6);
					trimWhitespace(p.name);

					int idx = p.name.find(" ");
					if (idx != -1)
					{
						p.description = p.name.substr(idx + 1);
						p.name.erase(idx);
					}

					trimWhitespace(p.description);
					line = "";
					comment.parameter.push_back(p);

					// cout << "Found parameter!" << endl;
					// cout << p.name << ": " << p.description << endl;
				}
				else if (line.find("@return") == 0)
				{
					comment.returnDescription = line.substr(7);
					trimWhitespace(comment.returnDescription);
				}
				else
					// FIXME: Make this html agnostic!
					comment.description += line + "<br>\n";

				std::getline(in, line);
				trimWhitespace(line);

				if (line.find("function") == 0)
				{
					comment.functionName = line;
				}

			} while (line.find("--") == 0);

			output += commentToString(comment);
		}
	}

	in.close();
	return output;
}

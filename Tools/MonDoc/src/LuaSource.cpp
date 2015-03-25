#include "LuaSource.h"
#include <iostream>
#include <vector>
#include <fstream>

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
		output += "<h1>";
		int idx = c.description.find("\n");

		string title = c.description;
		if (idx != -1)
		{
			title.erase(idx);
			output += title + "</h1><p>\n\n";

			c.description = c.description.substr(idx);
		}
	}

	output += c.description + "</p>";

	int numParams = c.parameter.size();

	if (numParams > 0)
		output += "<p>Parameter:<br>";

	Parameter p;
	for (int i = 0; i < numParams; i++)
	{
		p = c.parameter[i];
		output += "<p>" + p.name + "</p>\n";
		output += "<p class='indented'>" + p.description + "</p>\n";
	}

	if (numParams > 0)
		output += "</p>";

	if (!c.returnDescription.empty())
		output += "<p> Return: " + c.returnDescription + "</p>";

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
				trimString(line, '-');
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

//========================================================================
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

#include "../MainWindow/Translator.h"
#include <NeoEngine.h>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

const char* tr(const char* key)
{
    return Translator::getInstance()->translate(key);
}

void Translator::loadTranslation(const char* file, char delim)
{
   	/*
    SAFE_DELETE(m_parser);
    m_parser = new INI::Parser(file);
	*/

	ifstream in(file);
	if (!in)
	{
		MLOG_ERROR("Could not load translation from '" << file << "'!");
		return;
	}

	m_phrases.clear();
	
	string line, key, value;
	while(in)
		{
			getline(in, line);

			stringstream ss(line);
			getline(ss, key, delim);
			getline(ss, value, delim);
			
			m_phrases[key] = value;
		}
	
	string filename = file;
#ifdef WIN32
	std::replace(filename.begin(), filename.end(), '\\', '/');
#endif

	int idx = filename.find_last_of("/");
	if (idx != -1)
		m_langFile = filename.substr(idx+1);
	else
		m_langFile = filename;
    
	MLOG_INFO("Loaded translation from '" << m_langFile << "'");
}

const char* Translator::translate(const char* key)
{
    std::string value = m_phrases[key];
    if(value.empty())
        return key;

    return value.c_str();
}

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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "../MainWindow/ini.h"
#include <map>
#include <NeoCore.h>
#include <string>

const char* tr(const char* key);

class Translator
{
	std::string m_langFile;
	std::map<std::string, Neo::String*> m_phrases;
	
public:
    static Translator* getInstance() { static Translator m_tr; return &m_tr; }

    Translator() {}
	~Translator(){ clear(); }

    void loadTranslation(const char* file, char delim = ',');
    const char* translate(const char* key);
    const char* getLanguageFile() { return m_langFile.c_str(); }

	void clear();
};

#endif

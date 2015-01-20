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

const char* tr(const char* key);

class Translator
{
    INI::Parser* m_parser;
    char m_langFile[255];

public:
    static Translator* getInstance() { static Translator m_tr; return &m_tr; }

    Translator(): m_parser(NULL) {}
    void loadTranslation(const char* file);
    const char* translate(const char* key);
    const char* getLanguageFile() { return m_langFile; }
};

#endif

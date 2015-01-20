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

const char* tr(const char* key)
{
    return Translator::getInstance()->translate(key);
}

void Translator::loadTranslation(const char* file)
{
    if(!isFileExist(file))
		return;

    SAFE_DELETE(m_parser);
    m_parser = new INI::Parser(file);

#ifndef WIN32
    const char* substr = strrchr(file, '/');
#else
    const char* substr = strrchr(file, '\\');
#endif

    if(substr == NULL)
        substr = file;

    strcpy(m_langFile, ++substr);
}

const char* Translator::translate(const char* key)
{
    if(!m_parser)
        return key;

    std::string value = m_parser->top()("Keys")[key];
    if(value.empty())
        return key;

    return value.c_str();
}

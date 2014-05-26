/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MEditorTheme.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Copyright (c) 2013 Yannick Pflanzer <yp1995@live.de>
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

#ifndef MEDITOR_THEME_H
#define MEDITOR_THEME_H

#include <string>

struct MThemeColor
{
	float r, g, b, a;
};

class MEditorTheme
{
	std::string m_filepath;
	std::string m_name;
	
	struct MThemeColor m_normalColor;
	struct MThemeColor m_highlightColor;
	struct MThemeColor m_pressColor;
	struct MThemeColor m_normalaColor;
	struct MThemeColor m_menuColor;
	struct MThemeColor m_backgroundColor;
	struct MThemeColor m_toolbarColor;
	struct MThemeColor m_titleColor;
	
public:
	MEditorTheme();
	MEditorTheme(std::string themefile);
	void loadTheme(std::string themefile);
	
	std::string getPath() {return m_filepath;}
	
	struct MThemeColor getNormalColor() {return m_normalColor;}
	struct MThemeColor getHighlightColor() {return m_highlightColor;}
	struct MThemeColor getPressColor() {return m_pressColor;}
	struct MThemeColor getNormalAColor() {return m_normalaColor;}
	struct MThemeColor getMenuColor() {return m_menuColor;}
	struct MThemeColor getBackgroundColor() {return m_backgroundColor;}
	struct MThemeColor getToolbarColor() {return m_toolbarColor;}
	struct MThemeColor getTitleColor() {return m_titleColor;}
};

#endif

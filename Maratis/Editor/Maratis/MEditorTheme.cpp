/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MEditorTheme.cpp
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

#include <tinyxml.h>
#include <iostream>
#include "MEditorTheme.h" 

using namespace std;

MEditorTheme::MEditorTheme()
{
	
}

MEditorTheme::MEditorTheme(std::string themefile)
{
	 loadTheme(themefile);
}

void MEditorTheme::loadTheme(std::string themefile)
{
	TiXmlDocument theme(themefile.c_str());
	 
	 if(!theme.LoadFile())
	 {
		 cout << "Error: Could not load editor theme!" << endl;
		 exit(-1);
	 }
	 
	 TiXmlHandle hDoc(&theme);
	 TiXmlElement* rootNode;
	 TiXmlHandle hRoot(0);
	 
	 rootNode = hDoc.FirstChildElement().Element();
	 
	 if(!rootNode)
	 {
		 cout << "Error: Theme file is invalid!" << endl;
		 exit(-1);
	 }
	 
	 m_name = rootNode->Value();
 
	 TiXmlElement* filePath = rootNode->FirstChildElement("ImagePath");
	 if(filePath)
	 {
		 m_filepath = filePath->Attribute("file");
	 }
	 else
	 {
		 m_filepath = "default";
	 }
	 
	 TiXmlElement* color = rootNode->FirstChildElement("NormalColor");
	 if(color)
	 {
		 m_normalColor.r = atof(color->Attribute("r"));
		 m_normalColor.g = atof(color->Attribute("g"));
		 m_normalColor.b = atof(color->Attribute("b"));
		 m_normalColor.a = atof(color->Attribute("a"));
	 }
	 
	 color = rootNode->FirstChildElement("HighlightColor");
	 if(color)
	 {
		 m_highlightColor.r = atof(color->Attribute("r"));
		 m_highlightColor.g = atof(color->Attribute("g"));
		 m_highlightColor.b = atof(color->Attribute("b"));
		 m_highlightColor.a = atof(color->Attribute("a"));
	 }
	 
	 color = rootNode->FirstChildElement("PressColor");
	 if(color)
	 {
		 m_pressColor.r = atof(color->Attribute("r"));
		 m_pressColor.g = atof(color->Attribute("g"));
		 m_pressColor.b = atof(color->Attribute("b"));
		 m_pressColor.a = atof(color->Attribute("a"));
	 }
	 
	 color = rootNode->FirstChildElement("NormalColorA");
	 if(color)
	 {
		 m_normalaColor.r = atof(color->Attribute("r"));
		 m_normalaColor.g = atof(color->Attribute("g"));
		 m_normalaColor.b = atof(color->Attribute("b"));
		 m_normalaColor.a = atof(color->Attribute("a"));
	 }
	 
	 color = rootNode->FirstChildElement("MenuColor");
	 if(color)
	 {
		 m_menuColor.r = atof(color->Attribute("r"));
		 m_menuColor.g = atof(color->Attribute("g"));
		 m_menuColor.b = atof(color->Attribute("b"));
		 m_menuColor.a = atof(color->Attribute("a"));
	 }
	 
	 color = rootNode->FirstChildElement("BackgroundColor");
	 if(color)
	 {
		 m_backgroundColor.r = atof(color->Attribute("r"));
		 m_backgroundColor.g = atof(color->Attribute("g"));
		 m_backgroundColor.b = atof(color->Attribute("b"));
	 }
	 
	 color = rootNode->FirstChildElement("ToolbarColor");
	 if(color)
	 {
		 m_toolbarColor.r = atof(color->Attribute("r"));
		 m_toolbarColor.g = atof(color->Attribute("g"));
		 m_toolbarColor.b = atof(color->Attribute("b"));
		 m_toolbarColor.a = atof(color->Attribute("a"));
	 }
	 
	 color = rootNode->FirstChildElement("TitleColor");
	 if(color)
	 {
		 m_titleColor.r = atof(color->Attribute("r"));
		 m_titleColor.g = atof(color->Attribute("g"));
		 m_titleColor.b = atof(color->Attribute("b"));
		 m_titleColor.a = atof(color->Attribute("a"));
	 }
}

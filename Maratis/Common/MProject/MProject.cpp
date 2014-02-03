/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MProject.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include <MEngine.h>
#include <tinyxml.h>

#include "MProject.h"

const char* s_embedded_game_name  = "[EMBEDDED MPROJECT GAME NAME HERE]                             ";
const char* s_embedded_level_name = "[EMBEDDED MPROJECT LEVEL NAME HERE]                            ";
const char* s_embedded_renderer   = "[EMBEDDED MPROJECT RENDERER]                                   ";

bool MProject::loadXML(const char * filename)
{
	char rep[256];

	if(! filename)
		return false;

	// get rep
	getRepertory(rep, filename);

	// read document
	TiXmlDocument doc(filename);
	if(! doc.LoadFile())
		return false;
	
	TiXmlHandle hDoc(&doc);
	TiXmlElement * rootNode;
	TiXmlHandle hRoot(0);

	// maratis
	rootNode = hDoc.FirstChildElement().Element();
	if(! rootNode)
		return false;

	if(strcmp(rootNode->Value(), "Maratis") != 0)
		return false;

	hRoot = TiXmlHandle(rootNode);

	// Project
	TiXmlElement * projectNode = rootNode->FirstChildElement("Project");
	if(! projectNode)
		return false;

	// renderer
	TiXmlElement * rendererNode = projectNode->FirstChildElement("renderer");
	if(rendererNode)
	{
		const char * name = rendererNode->Attribute("name");
		if(name)
		{
			renderer = name;
		}
	}
	
	// start
	TiXmlElement * startNode = projectNode->FirstChildElement("start");
	if(startNode)
	{
		const char * file = startNode->Attribute("file");
		if(file)
		{		
			char levelFilename[256];
			getGlobalFilename(levelFilename, rep, file);
			startLevel = levelFilename;
		}
	}


	return true;
}

bool MProject::saveXML(const char * filename)
{
	FILE * file = fopen(filename, "wt");
	if(! file)
		return false;

	// get rep
	char rep[256];
	getRepertory(rep, filename);

	char version[] = "3.0";
	fprintf(file, "<Maratis version=\"%s\">\n\n", version);
	fprintf(file, "<Project>\n\n");

	// renderer
	if(renderer.size() > 0)
	{
		fprintf(file, "\t<renderer name=\"%s\" />\n", renderer.c_str());
	}
	
	// start
	{
		// file
		char levelFilename[256];
		getLocalFilename(levelFilename, rep, startLevel.c_str());
		fprintf(file, "\t<start file=\"%s\" />\n\n", levelFilename);
	}

	fprintf(file, "</Project>\n\n");
	fprintf(file, "</Maratis>");

	fclose(file);
	return true;
}

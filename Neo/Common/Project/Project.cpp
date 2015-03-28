//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#include <NeoEngine.h>
#include <tinyxml.h>

#include "Project.h"

using namespace Neo;

namespace Neo
{
const char* s_embedded_game_name  = "[EMBEDDED MPROJECT GAME NAME HERE]                             ";
const char* s_embedded_level_name = "[EMBEDDED MPROJECT LEVEL NAME HERE]                            ";
const char* s_embedded_renderer   = "[EMBEDDED MPROJECT RENDERER]                                   ";
}

bool Project::loadXML(const char * filename)
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

	if (strcmp(rootNode->Value(), "Neo") != 0)
	{
		if (strcmp(rootNode->Value(), "Maratis") == 0)
		{
			MLOG_WARNING(
				"You try to load a project that was created using Maratis.");
		}
		else
		{
			MLOG_ERROR("Can't load project: File is invalid!");
			return false;
		}
	}

	hRoot = TiXmlHandle(rootNode);

    // Game config
    TiXmlElement * gameNode = rootNode->FirstChildElement("Game");
    NeoGame* game = NeoEngine::getInstance()->getGame();
    if(gameNode)
    {
       const char* vertShad = gameNode->Attribute("vertex_shader");
       const char* fragShad = gameNode->Attribute("fragment_shader");
       const char* resolution_string = gameNode->Attribute("resolution");

       float resolution = 1.0;

       if(resolution_string)
            sscanf(resolution_string, "%f", &resolution);

       if(game)
       {
           game->getPostProcessor()->setShaderPath(vertShad, fragShad);
           game->getPostProcessor()->setResolutionMultiplier(resolution);
           game->enablePostEffects();
       }
    }
    else if(game)
    {
        game->disablePostEffects();
    }

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

bool Project::saveXML(const char * filename)
{
	FILE * file = fopen(filename, "wt");
	if(! file)
		return false;

	// get rep
	char rep[256];
	getRepertory(rep, filename);

	/// BUG: Should use the real version!
	char version[] = "0.4";
	fprintf(file, "<Neo version=\"%s\">\n\n", version);

    // Write game config (post effects etc.)
    NeoGame* game = NeoEngine::getInstance()->getGame();
    if(game != NULL && game->hasPostEffects())
    {
        fprintf(file, "<Game vertex_shader=\"%s\" fragment_shader=\"%s\" resolution=\"%f\"/>\n",
                game->getPostProcessor()->getVertexShader(), game->getPostProcessor()->getFragmentShader(),
                game->getPostProcessor()->getResolutionMultiplier());
    }

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
	fprintf(file, "</Neo>");

	fclose(file);
	return true;
}

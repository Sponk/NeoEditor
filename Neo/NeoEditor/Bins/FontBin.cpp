/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MFontBin.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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

#include "FontBin.h"

using namespace Neo;

char M_FONT_HEADER[8] = {'M', 'F', 'O', 'N', 'T', '\0', '\0', '\0'};


bool exportFontBin(const char * filename, Font * font)
{
	if(! font)
		return false;

	NeoEngine * engine = NeoEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	// read image
	MImage image;
	render->bindTexture(font->getTextureId());
	render->getTexImage(0, &image);
	
	unsigned int width = image.getWidth();
	unsigned int height = image.getHeight();

	if(width == 0 && height == 0)
	{
		fprintf(stderr, "Error : unable to create image font for %s\n", filename);
		return false;
	}

	// create file
	FILE * file = fopen(filename, "wb");
	if(! file)
	{
		fprintf(stderr, "Error : can't create file %s\n", filename);
		return false;
	}
	
	// bin
	fwrite(M_FONT_HEADER, sizeof(char), 8, file);

	// version
	int version = 1;
	fwrite(&version, sizeof(int), 1, file);

	// font size
	unsigned int fontSize = font->getFontSize();
	fwrite(&fontSize, sizeof(int), 1, file);

	// write image
	{
		fwrite(&width, sizeof(int), 1, file);
		fwrite(&height, sizeof(int), 1, file);

		unsigned char color[4];
		unsigned int x, y;
		for(y=0; y<height; y++)
		{
			for(x=0; x<width; x++)
			{
				image.readPixel(x, y, color);
				fwrite(&color[3], sizeof(char), 1, file);
			}
		}
	}

	// write characters infos
	{
		map <unsigned int, Character> * characters = font->getCharacters();

		// size
		unsigned int size = font->getCharactersNumber();
		fwrite(&size, sizeof(int), 1, file);

		// characters
		map <unsigned int, Character>::iterator
			mit (characters->begin()),
			mend(characters->end());

		for(;mit!=mend;++mit)
		{
			unsigned int charCode = mit->first;
			Character * character = &mit->second;

			MVector2 pos = character->getPos();
			MVector2 offset = character->getOffset();
			MVector2 scale = character->getScale();
			float xadvance = character->getXAdvance();

			fwrite(&charCode, sizeof(int), 1, file);
			fwrite(&pos, sizeof(float), 2, file);
			fwrite(&offset, sizeof(float), 2, file);
			fwrite(&scale, sizeof(float), 2, file);
			fwrite(&xadvance, sizeof(float), 1, file);
		}
	}

	fclose(file);
	return true;
}

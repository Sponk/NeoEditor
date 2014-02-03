/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MFreetypeLoader.cpp
//
// Freetype font loader
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


#include <ft2build.h>
#include FT_FREETYPE_H 

#include <MEngine.h>

#include "MFreetypeLoader.h"


static void drawBitmap(MImage * image, FT_Bitmap * bitmap, int left, int top)
{
	int x, y;
	int width = bitmap->width;
	int height = bitmap->rows;
	int imgWidth = (int)image->getWidth();
	int imgHeight = (int)image->getHeight();

	unsigned char * pixel = bitmap->buffer;

	for(y=top; y<top+height; y++)
	{
		for(x=left; x<left+width; x++)
		{
			if(y >= 0 && y < imgHeight && 
			   x >= 0 && x < imgWidth)
			{
				unsigned char color[4] = {255, 255, 255, (*pixel)};
				image->writePixel((unsigned int)x, (unsigned int)y, color);
			}
			pixel++;
		}
	}
}

bool M_loadFont(const char * filename, void * data)
{
	int pen_x, pen_y;
	unsigned int n;
	unsigned int space = 2;
	unsigned int size = 32;
	unsigned int width = 512;
	unsigned int height = 0;

	MImage image;

	FT_GlyphSlot slot;
	FT_Library library;
	FT_Face face;
	FT_Byte * file_base;
	FT_Long file_size;


	// init
	FT_Error error = FT_Init_FreeType(&library); 
	if(error){
		printf("ERROR Load Font : unable to init FreeType\n");
		return false;
	}
	
	
	// open file
	MFile * file = M_fopen(filename, "rb");
	if(! file)
	{
		FT_Done_FreeType(library);
		printf("ERROR Load Font : can't read file %s\n", filename);
		return false;
	}
	
	M_fseek(file, 0, SEEK_END);
	file_size = M_ftell(file);
	M_rewind(file);
	
	file_base = new FT_Byte[file_size];
	if(file_size != M_fread(file_base, sizeof(FT_Byte), file_size, file))
	{
		M_fclose(file);
		FT_Done_FreeType(library);
		delete [] file_base;
		return false;
	}
	
	
	// read font
	error = FT_New_Memory_Face(library, file_base, file_size, 0, &face);
	M_fclose(file);
	
	if(error)
	{
		printf("ERROR Load Font : unable to read data %s\n", filename);
		FT_Done_FreeType(library);
		delete [] file_base;
		return false;
	}

	// set font size
	error = FT_Set_Pixel_Sizes(face, 0, size);
	if(error)
	{
		printf("ERROR Load Font : unable to size font\n");
		FT_Done_FreeType(library);
		delete [] file_base;
		return false;
	}


	// parse characters
	slot = face->glyph;
	pen_x = space; pen_y = space; 
	for(n = 0; n<256; n++)
	{
		// load glyph image into the slot (erase previous one)
		error = FT_Load_Char(face, n, FT_LOAD_RENDER); 

		if(error) 
			continue;

		if(FT_Get_Char_Index(face, n) == 0)
			continue;

		if((pen_x + slot->bitmap.width) > 512){
			pen_x = 0;
			pen_y += size + space;
			height += size + space;
		}

		// increment pen position 
		pen_x += slot->bitmap.width + space; 
	}

	if(height == 0)
	{
		printf("ERROR Load Font : unable to create font texture\n");
		FT_Done_FreeType(library);
		delete [] file_base;
		return false;
	}


	// create image
	height = getNextPowerOfTwo(height);
	image.create(M_UBYTE, width, height, 4);

	unsigned char color[4] = {255, 255, 255, 0};
	image.clear(color);


	// init font
	MFont * font = (MFont *)data;
	font->setFontSize(size);
	font->setTextureWidth(width);
	font->setTextureHeight(height);


	// create font texture
	slot = face->glyph;
	pen_x = space; pen_y = space; 
	for(n = 0; n<256; n++)
	{
		// load glyph image into the slot (erase previous one)
		error = FT_Load_Char(face, n, FT_LOAD_RENDER); 

		if(error) 
			continue;

		if(FT_Get_Char_Index(face, n) == 0)
			continue;

		if((pen_x + slot->bitmap.width) > (int)image.getWidth()){
			pen_x = 0;
			pen_y += size + space;
		}

		// get character properties
		float xAdvance = (slot->advance.x >> 6) / ((float)size);
		MVector2 offset = MVector2((float)slot->bitmap_left, - (float)slot->bitmap_top) / ((float)size);
		MVector2 pos = MVector2((float)(pen_x-1) / (float)width, (float)(pen_y-1) / (float)height);
		MVector2 scale = MVector2((float)(slot->bitmap.width+2) / (float)width, (float)(slot->bitmap.rows+2) / (float)height);

		// set character
		font->setCharacter(n, MCharacter(xAdvance, offset, pos, scale));

		// draw to image
		drawBitmap(&image, &slot->bitmap, pen_x, pen_y);

		// increment pen position 
		pen_x += slot->bitmap.width + space; 
	} 


	// send texture
	MEngine * engine = MEngine().getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	// gen texture id
	unsigned int textureId = font->getTextureId();
	if(textureId == 0)
	{
		render->createTexture(&textureId);
		font->setTextureId(textureId);
	}
	
	// send texture image
	render->bindTexture(textureId);
	render->setTextureUWrapMode(M_WRAP_REPEAT);
	render->setTextureVWrapMode(M_WRAP_REPEAT);
	render->sendTextureImage(&image, 0, 1, 0);

	// finish
	FT_Done_FreeType(library);
	delete [] file_base;
	return true;
}
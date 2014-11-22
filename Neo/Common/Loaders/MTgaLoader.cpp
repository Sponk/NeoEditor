/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MTgaLoader.cpp
//
// TGA image loader
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
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

#include <MCore.h>
#include <MEngine.h>
#include "MTgaLoader.h"

static void flipImage(MImage * source)
{
	unsigned int y;
	unsigned int width = source->getWidth();
	unsigned int height = source->getHeight();
	unsigned int components = source->getComponents();

	MImage copy;
	copy.create(M_UBYTE, width, height, components);
	memcpy(copy.getData(), source->getData(), sizeof(char)*source->getSize());
	void * data = copy.getData();
	void * resultData = source->getData();

	for(y=0; y<height; y++)
		memcpy((char*)resultData + y*width*components, (char*)data + (height - y - 1)*width*components, sizeof(char)*width*components);
}

#pragma pack(push, 1)
typedef struct
{
	unsigned char id_length;
	unsigned char color_map_type;
	unsigned char image_type;

	// Color map spec
	unsigned short cm_first_entry_index;
	unsigned short cm_length;
	unsigned char cm_entry_size;

	// Image spec
	unsigned short im_x_origin;
	unsigned short im_y_origin;
	unsigned short im_width;
	unsigned short im_height;
	unsigned char im_pixel_depth;
	unsigned char im_descriptor;
} Header;
#pragma pack(pop)

bool M_loadTgaImage(const char * filename, void * data)
{
	if((!data) || (!filename))
		return false;

	MFile* file = M_fopen(filename, "rb");
	if (!file)
	{
        //fprintf(stderr, "ERROR Load TGA : unable to open %s\n", filename);
		return false;
	}

	Header header;
	if (M_fread(&header, sizeof(Header), 1, file) == 0)
	{
        MLOG_ERROR("Load TGA: Could not read header from \"" << filename << "\"");
        //fprintf(stderr, "ERROR Load TGA : could not read header from '%s'\n", filename);
		return false;
	}

	switch(header.color_map_type)
	{
		case 0: // no color map
			if (header.cm_first_entry_index == 0 ||
				header.cm_length == 0 || header.cm_entry_size == 0)
				break;
		case 1: // color map present
		default:
			return false;
	}

	bool compression;
	switch(header.image_type)
	{
		case 2: // uncompressed bgr / bgra
			compression = false;
			break;
		case 10: // rle encoded bgr / bgra
			compression = true;
			break;
		case 0: // no image data present
		case 1: // uncompressed color mapped
		case 3: // uncompressed grayscale
		case 9: // rle encoded color mapped
		case 11: // rle encoded grayscale
		default:
			return false;
	}

	unsigned int components;
	switch(header.im_pixel_depth)
	{
		case 24:
			components = 3;
			break;
		case 32:
			components = 4;
			break;
		case 8:
		default:
			return false;
	}

	bool flip = (header.im_descriptor & (1<<5)) ? false : true;

	unsigned char* image_id = new unsigned char[header.id_length];
	M_fread(image_id, sizeof(unsigned char), header.id_length, file);
	delete [] image_id;

	unsigned char* color_map_data = new unsigned char[header.cm_length];
	M_fread(color_map_data, sizeof(unsigned char), header.cm_length, file);
	delete [] color_map_data;

	const long size = header.im_width * header.im_height * components;
	unsigned char* image_data = new unsigned char[size];

	if (compression)
	{
		unsigned char chunk_header;
		unsigned char* chunk_data = new unsigned char[128];
		unsigned int offset = 0;
        char* color_buf = new char[components];

		do
		{            
			M_fread(&chunk_header, sizeof(unsigned char), 1, file);
			if (chunk_header < 128)
			{
                chunk_header++;
                /*M_fread(chunk_data, sizeof(unsigned char), chunk_header * components, file);
                memcpy(&image_data[offset], chunk_data, chunk_header * components);*/

                for(int i = 0; i < chunk_header; i++)
                {
                    M_fread(color_buf, 1, components, file);

                    image_data[offset] = color_buf[0];
                    image_data[offset + 1] = color_buf[1];
                    image_data[offset + 2] = color_buf[2];

                    if(components == 4)
                        image_data[offset + 3] = color_buf[3];

                    offset += components;
                }

                //offset += chunk_header * components;
			}
			else
			{
				chunk_header -= 127;
				M_fread(chunk_data, sizeof(unsigned char), components, file);

                for (int i = 0; i < chunk_header; i++)
				{
                    memcpy(&image_data[offset], chunk_data, components);
					offset += components;
				}
			}
		}
		while (offset < size);

        delete color_buf;
	}
	else
	{
		M_fread(image_data, sizeof(unsigned char), size, file);
	}

	M_fclose(file);

	for(int i = 0; i < size; i += components)
		image_data[i] ^= image_data[i+2] ^= image_data[i] ^= image_data[i+2];

	MImage * image = (MImage *)data;
	image->create(M_UBYTE, (unsigned int)header.im_width, (unsigned int)header.im_height, components);
	memcpy(image->getData(), image_data, sizeof(char)*image->getSize());
	delete [] image_data;

	if (flip)
		flipImage(image);

	return true;
}

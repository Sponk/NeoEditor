/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MPngLoader.cpp
//
// PNG image loader
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

#include <png.h>

#include <MCore.h>
#include "MPngLoader.h"

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    M_fread(data, 1, length, (MFile *)png_get_io_ptr(png_ptr));
}

bool M_loadPngImage(const char * filename, void * data)
{
    if((!data) || (!filename))
        return false;

    MFile* file = M_fopen(filename, "rb");
    if (!file)
    {
        //fprintf(stderr, "ERROR Load PNG : unable to open %s\n", filename);
        return false;
    }

    unsigned char* header = new unsigned char[8];
    M_fread(header, sizeof(char), 8, file);
    if (png_sig_cmp(header, 0, 8))
    {
        M_fclose(file);
        delete [] header;
        return false;
    }
    M_rewind(file);
    delete [] header;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    png_set_read_fn(png_ptr, (void *)file, user_read_data);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    unsigned int components;
    switch(color_type)
    {
        case PNG_COLOR_TYPE_RGB:
            components = 3;
            break;
        case PNG_COLOR_TYPE_RGBA:
            components = 4;
            break;
        default:
			fprintf(stderr, "ERROR Load PNG : unsupported color space in %s\n", filename);
            return false;
    }

    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    unsigned char* out = new unsigned char[row_bytes * height];
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    for (unsigned int i = 0; i < height; i++)
        memcpy(out+(row_bytes * i), row_pointers[i], row_bytes);

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    M_fclose(file);

    MImage * image = (MImage *)data;
    image->create(M_UBYTE, (unsigned int)width, (unsigned int)height, components);
    memcpy(image->getData(), out, sizeof(char)*image->getSize());
    delete [] out;

    return true;
}

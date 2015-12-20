//========================================================================
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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

#include <NeoCore.h>
#include <png.h>

#include "PngSaver.h"

namespace Neo
{
void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    M_fwrite(data, 1, length, (File*) png_get_io_ptr(png_ptr));
}

void user_fflush(png_struct_def* png_ptr)
{

}

bool M_savePngImage(const char* filename, const char* type, Image* image)
{
    if((!image) || (!filename) || !type || strcmp(type, "png"))
        return false;

    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte** row_pointers = NULL;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png_ptr)
    {
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if(setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    unsigned int color_type = 0;

    switch(image->getComponents())
    {
        case 1: color_type = PNG_COLOR_TYPE_GRAY;
            break;

        case 3: color_type = PNG_COLOR_TYPE_RGB;
            break;

        case 4: color_type = PNG_COLOR_TYPE_RGBA;
            break;

        default:
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return false;
    }

    png_set_IHDR(png_ptr,
                 info_ptr,
                 image->getWidth(),
                 image->getHeight(),
                 8*image->getPixelSize(),
                 color_type,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    File* file = M_fopen(filename, "wb");
    if (!file)
    {
        //fprintf(stderr, "ERROR Load PNG : unable to open %s\n", filename);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    unsigned int lineOffset = image->getPixelSize() * image->getWidth() * image->getComponents();
    row_pointers = (png_byte**) png_malloc(png_ptr, image->getHeight() * sizeof(png_byte*));
    for(int y = 0; y < image->getHeight(); y++)
        row_pointers[y] = (png_byte*) image->getData() + lineOffset;

    png_set_write_fn(png_ptr, (void*) file, user_write_data, user_fflush);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    png_free(png_ptr, row_pointers);

    M_fclose(file);

    return true;
}
}

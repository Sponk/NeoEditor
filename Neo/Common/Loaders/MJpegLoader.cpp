/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MJpegLoader.cpp
//
// JPEG image loader
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

#include <stdio.h>
#include <setjmp.h>

#include <jpeglib.h>
#include <jerror.h>

#include <MCore.h>
#include <MEngine.h>
#include "MJpegLoader.h"

struct my_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	longjmp(myerr->setjmp_buffer, 1);
}

bool M_loadJpegImage(const char * filename, void * data)
{
    if((!data) || (!filename))
        return false;

	MFile* file = M_fopen(filename, "rb");
	if (!file)
	{
        //fprintf(stderr, "ERROR Load JPEG : unable to open %s\n", filename);
		return false;
	}

    M_fseek(file, 0, SEEK_END);
    long filesize = M_ftell(file);
    M_rewind(file);

    unsigned char* buffer = new unsigned char[filesize];
    if (filesize != M_fread(buffer, sizeof(char), filesize, file))
    {
        //fprintf(stderr, "ERROR Load JPEG : unable to read %s\n", filename);
        M_fclose(file);
        delete [] buffer;
        return false;
    }

    M_fclose(file);

	struct jpeg_decompress_struct cinfo;
	jpeg_create_decompress(&cinfo);

    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        delete [] buffer;
        return false;
    }

    jpeg_mem_src(&cinfo, &buffer[0], filesize);
	jpeg_read_header(&cinfo, 1);

	switch(cinfo.out_color_space)
	{
		case JCS_RGB:
			break;
		default:
            //fprintf(stderr, "ERROR Load JPEG : unsupported color space in %s\n", filename);
            MLOG_ERROR("Load JPEG: Unsupported color space in \"" << filename << "\"");
            delete [] buffer;
			return false;
	}

    jpeg_start_decompress(&cinfo);

    unsigned char* out = new unsigned char[cinfo.output_width * cinfo.output_height * cinfo.output_components];
    int row_stride = cinfo.output_width * cinfo.output_components;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *rowp[1];
        rowp[0] = (unsigned char *) out + row_stride * cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, rowp, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    delete [] buffer;

    MImage * image = (MImage *)data;
    image->create(M_UBYTE, (unsigned int)cinfo.output_width, (unsigned int)cinfo.output_height, (unsigned int)cinfo.output_components);
    memcpy(image->getData(), out, sizeof(char)*image->getSize());
    delete [] out;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MPngLoader.h
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


#ifndef _M_PNG_IMAGE_LOADER_H
#define _M_PNG_IMAGE_LOADER_H

namespace Neo
{
/**
 * @brief Loads a PNG image file and stores the pixel data in the given pointer.
 *
 * @param filename The path to the image to load.
 * @param data A pointer to allocated memory.
 * @return Returns <b>true</b> on success and <b>false</b> on failure.
 */
bool M_loadPngImage(const char * filename, void * data);
}
#endif

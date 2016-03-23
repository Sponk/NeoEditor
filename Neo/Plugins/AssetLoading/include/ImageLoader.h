//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __IMAGE_LOADER_H
#define __IMAGE_LOADER_H

#include "config.h"

namespace Neo
{
/**
 * @brief Loads any supported image file and stores the pixel data in the given pointer.
 *
 * This function calls all possible image loaders sequentially to figure out the right
 * format to load. Returns false if all image loader fail.
 *
 * @param filename The path to the image to load.
 * @param data A pointer to allocated memory.
 * @return Returns the success value.
 */
bool EXPORT M_loadImage(const char * filename, Image* image);
}
#endif

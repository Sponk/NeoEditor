//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#ifndef __MESH_LOAD_H
#define __MESH_LOAD_H

#include "config.h"

namespace Neo
{
bool NEO_COMMON_EXPORT xmlArmatureAnimLoad(const char * filename, ArmatureAnim* armatureAnim);
bool NEO_COMMON_EXPORT xmlTextureAnimLoad(const char * filename, TexturesAnim* texturesAnim);
bool NEO_COMMON_EXPORT xmlMaterialAnimLoad(const char * filename, MaterialsAnim* materialsAnim);
bool NEO_COMMON_EXPORT xmlMeshLoad(const char * filename, Mesh* mesh);
}
#endif

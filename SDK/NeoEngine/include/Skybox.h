//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <www.scary-squid.de>
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


#ifndef __SKYBOX_H
#define __SKYBOX_H

#include <NeoCore.h>

namespace Neo
{
class NEO_ENGINE_EXPORT Skybox
{
    void drawQuad(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Matrix4x4* matrix, Vector2* texCoords);

    TextureRef* m_SkyboxTexture[6];

    bool m_init;

    unsigned int m_fx;
    unsigned int m_vertShad;
    unsigned int m_pixShad;

    String m_path;

public:
    Skybox();
    // destructor
    ~Skybox(void){}

    void drawSkybox(Vector3 position, Vector3 rotation);
    void loadSkyboxTextures(const char* path);

    inline const char* getPath() { return m_path.getSafeString(); }
    inline bool isInitialized() { return m_init; }
};
}
#endif

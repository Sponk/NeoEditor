/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MSkybox.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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


#ifndef _M_SKYBOX_H
#define _M_SKYBOX_H

#include <MCore.h>

class M_ENGINE_EXPORT MSkybox
{
    void drawQuad(MVector3 v1, MVector3 v2, MVector3 v3, MVector3 v4, MMatrix4x4* matrix, MVector2* texCoords);

    MTextureRef* m_SkyboxTexture[6];

    bool m_init;

    unsigned int m_fx;
    unsigned int m_vertShad;
    unsigned int m_pixShad;

    MString m_path;

public:
    MSkybox();
    // destructor
    ~MSkybox(void){}

    void drawSkybox(MVector3 position, MVector3 rotation);
    void loadSkyboxTextures(const char* path);

    inline const char* getPath() { return m_path.getSafeString(); }
    inline bool isInitialized() { return m_init; }
};

#endif

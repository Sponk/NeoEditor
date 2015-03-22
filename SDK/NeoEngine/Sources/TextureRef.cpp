/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MTextureRef.cpp
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


#include "../Includes/NeoEngine.h"

using namespace Neo;

TextureRef::TextureRef(unsigned int textureId, const char * filename, bool mipmap):
	m_textureId(textureId),
	m_mipmap(mipmap),
	m_components(0),
	m_width(0),
	m_height(0)
{
	m_filename.set(filename);
}

TextureRef::~TextureRef(void)
{
	clear();
}

TextureRef * TextureRef::getNew(unsigned int textureId, const char * filename, bool mipmap)
{
	return new TextureRef(textureId, filename, mipmap);
}

void TextureRef::clear(void)
{
	NeoEngine::getInstance()->getRenderingContext()->deleteTexture(&m_textureId);
}

void TextureRef::destroy(void)
{
	delete this;
}

void TextureRef::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	RenderingContext * render = engine->getRenderingContext();

	Image image;
	if(engine->getImageLoader()->loadData(getFilename(), &image))
	{
		if(m_textureId == 0)
			render->createTexture(&m_textureId);

		m_components = image.getComponents();
		m_width = image.getWidth();
		m_height = image.getHeight();
				
		// send texture image
		render->bindTexture(m_textureId);
		render->sendTextureImage(&image, isMipmapEnabled(), 1, 0);
	}
}

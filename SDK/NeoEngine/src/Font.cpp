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


#include <NeoEngine.h>

using namespace Neo;

Character::Character(void):
	m_xAdvance(0),
	m_offset(0, 0),
	m_pos(0, 0),
	m_scale(0, 0)
{}


Font::Font(void):
m_textureId(0),
m_fontSize(0),
m_textureWidth(0),
m_textureHeight(0)
{}

Font::~Font(void)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	render->deleteTexture(&m_textureId);
}

Font * Font::getNew(void)
{
	return new Font();
}

void Font::destroy(void)
{
	delete this;
}

void Font::setCharacter(unsigned int charCode, const Character & character)
{
	m_characters[charCode] = character;
}

Character * Font::getCharacter(unsigned int charCode)
{
	map<unsigned int, Character>::iterator iter = m_characters.find(charCode);
	if(iter != m_characters.end())
		return &iter->second;

	return NULL;
}

unsigned int Font::getCharactersNumber(void)
{
	return (unsigned int)m_characters.size();
}

Character * Font::getCharacterByIndex(unsigned int id)
{
	map<unsigned int, Character>::iterator iter(m_characters.begin());

	for(unsigned int i=0; i<id; i++)
		iter++;

	if(iter != m_characters.end())
		return &iter->second;

	return NULL;
}

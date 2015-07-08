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

ShaderRef::ShaderRef(unsigned int shaderId, M_SHADER_TYPES type, const char * filename):
	m_shaderId(shaderId),
	m_type(type),
	m_header("")
{
	m_filename.set(filename);
}

ShaderRef::~ShaderRef(void){
	clear();
}

ShaderRef * ShaderRef::getNew(unsigned int shaderId, M_SHADER_TYPES type, const char * filename){
	return new ShaderRef(shaderId, type, filename);
}

void ShaderRef::clear(void){
	NeoEngine::getInstance()->getRenderingContext()->deleteShader(&m_shaderId);
}

void ShaderRef::destroy(void){
	delete this;
}

void ShaderRef::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	Level * level = engine->getLevel();
	RenderingContext * render = engine->getRenderingContext();

	char * text = readTextFile(getFilename());
	if(text)
	{
		// gen shader if null
		if(m_shaderId == 0)
		{
			switch(m_type)
			{
			case M_SHADER_VERTEX:
				render->createVertexShader(&m_shaderId);
				break;
			case M_SHADER_PIXEL:
				render->createPixelShader(&m_shaderId);
				break;
			}
		}

		size_t sz = strlen(text) + strlen(m_header.getSafeString());
		char* sources = new char[sz];
		strcpy(sources, m_header.getSafeString());
		strcat(sources, text);

		// send shader source
		render->sendShaderSource(m_shaderId, sources);
		SAFE_FREE(text);
		SAFE_DELETE(sources);

		FXManager * fx_manager = level->getFXManager();

		unsigned int i;
		unsigned int size = fx_manager->getFXRefsNumber();
		for(i=0; i<size; i++)
		{
			FXRef * fx_ref = fx_manager->getFXRef(i);
			if((fx_ref->getPixelShaderRef() == this) || (fx_ref->getVertexShaderRef() == this))
			{
				unsigned int FXId = fx_ref->getFXId();
				render->updateFX(FXId);
			}
		}
	}
}

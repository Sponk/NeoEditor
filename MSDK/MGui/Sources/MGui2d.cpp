/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui2d.cpp
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


#include "../Includes/MGui.h"


MGui2d::MGui2d(void):
m_parentWindow(NULL),
m_isVisible(true),
m_isHighLight(false),
m_isPressed(false),
m_isDrawingText(true),
m_hasNormalTexture(false),
m_hasPressedTexture(false),
m_hasHighLightTexture(false),
m_textSize(16),
m_hasShadow(false),
m_isScrollAffect(true),
m_variableType(M_VAR_NONE),
m_variablePointer(NULL),
m_fontId(0),
m_centerText(false),
m_autoScaleFromText(true)
{
}

MGui2d::~MGui2d(void)
{
}

MGuiTextureFont * MGui2d::getFont(void)
{
	return MGui::getInstance()->getFont(m_fontId);
}

void MGui2d::setPosition(const MVector2 & position)
{
	m_position = position;
}

void MGui2d::setXPosition(float x)
{
	m_position.x = x;
}

void MGui2d::setYPosition(float y)
{
	m_position.y = y;
}

void MGui2d::setScale(const MVector2 & scale)
{
	m_scale = scale;
}

void MGui2d::setXScale(float x)
{
	m_scale.x = x;
}

void MGui2d::setYScale(float y)
{
	m_scale.y = y;
}

void MGui2d::autoScaleFromText(void)
{
	if(! isDrawingText())
		return;

	const char * text = getText();
	if(! text)
		return;

	setScale(
		getFont()->getMaxSize(
			getText(), 
			getTextSize()
		)
	);
}

void MGui2d::setNormalTexture(const char * filename)
{
	if(MGui::getInstance()->loadTexture(filename, &m_normalTexture))
	{
		if(! m_hasPressedTexture)
		{
			m_pressedTexture = m_normalTexture;
			m_hasPressedTexture = true;
		}

		if(! m_hasHighLightTexture)	
		{
			m_highLightTexture = m_normalTexture;
			m_hasHighLightTexture = true;
		}

		m_hasNormalTexture = true;
	}
}

void MGui2d::setPressedTexture(const char * filename)
{
	if(MGui::getInstance()->loadTexture(filename, &m_pressedTexture))
		m_hasPressedTexture = true;
}

void MGui2d::setHighLightTexture(const char * filename)
{
	if(MGui::getInstance()->loadTexture(filename, &m_highLightTexture))
		m_hasHighLightTexture = true;
}

void MGui2d::draw(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	if(! isVisible())
		return;

	if(isPressed()) // pressed
	{  
		render->setColor4(getPressedColor());

		if(hasPressedTexture())
		{
			render->enableTexture();
			drawTexturedQuad(getPressedTexture());
		}
		else
		{
			render->disableTexture();
			drawQuad();
		} 
	}
	else if(isHighLight()) // highLight
	{
		render->setColor4(getHighLightColor());

		if(hasHighLightTexture()){
			render->enableTexture();
			drawTexturedQuad(getHighLightTexture());
		}
		else
		{
			render->disableTexture();
			drawQuad();
		}
	}
	else // normal	
	{
		render->setColor4(getNormalColor());

		if(hasNormalTexture())
		{
			render->enableTexture();
			drawTexturedQuad(getNormalTexture());
		}	  
		else
		{
			render->disableTexture();
			drawQuad();
		}
	}

	if(isDrawingText() && (getText() != NULL))
	{
		MVector2 textDir;
		
		if(m_centerText)
		{
			MVector2 textScale = getFont()->getMaxSize(getText(), getTextSize());
			textDir = (m_scale - textScale)*0.5f;
		}
		
		render->enableTexture();
		render->setColor4(getTextColor());
		getFont()->draw(getText(), getPosition() + textDir, getTextSize());
	}

	// draw shadows
	if(hasShadow())
	{
		render->pushMatrix();
		render->translate(MVector3(getPosition().x, getPosition().y, 0));

		drawShadow();

		render->popMatrix();
	}
}

void MGui2d::drawShadow(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	MVector2 g_vertices[8];
	MVector4 g_colors[8];

	render->disableTexture();
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);

	MVector4 color0 = MVector4(0, 0, 0, 0);
	MVector4 color1 = MVector4(0, 0, 0, 0.05f);

	float size = 4;

	MVector2 dir[4];
	dir[0] = MVector2(size, size*0.4f);
	dir[1] = MVector2(size*0.4f, size);
	dir[2] = MVector2(size*0.4f, size*0.1f);
	dir[3] = MVector2(size*0.1f, size*0.4f);

	render->disableNormalArray();
	render->disableTexCoordArray();
	render->enableVertexArray();
	render->enableColorArray();

	render->setVertexPointer(M_FLOAT, 2, g_vertices);
	render->setColorPointer(M_FLOAT, 4, g_colors);

	for(int i=0; i<4; i++)
	{
		g_colors[0] = color1;
		g_vertices[0] = MVector2(0, m_scale.y) + MVector2(dir[i].x, 0);
		g_colors[1] = color0;
		g_vertices[1] = MVector2(0, m_scale.y) + MVector2(size, size) + dir[i];

		g_colors[2] = color1;
		g_vertices[2] = MVector2(m_scale.x, m_scale.y);
		g_colors[3] = color0;
		g_vertices[3] = MVector2(m_scale.x, m_scale.y) + MVector2(size, size) + dir[i];

		g_colors[4] = color1;
		g_vertices[4] = MVector2(m_scale.x, 0) + MVector2(0, dir[i].y);
		g_colors[5] = color0;
		g_vertices[5] = MVector2(m_scale.x, 0) + MVector2(size, size) + dir[i];

		render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 6);
	}
}

bool MGui2d::isPointInside(const MVector2 & point)
{
	MVector2 position = getPosition();
	MVector2 scale = getScale();

	if(point.x >= position.x && point.x < (position.x + scale.x) &&
	   point.y >= position.y && point.y < (position.y + scale.y))
	   return true;

	return false;
}

void MGui2d::drawQuad(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	MVector2 g_vertices[8];

	render->disableNormalArray();
	render->disableTexCoordArray();
	render->disableColorArray();
	render->enableVertexArray();

	g_vertices[0] = MVector2(m_position.x, m_position.y);
	g_vertices[1] = MVector2(m_position.x, m_position.y + m_scale.y);
	g_vertices[3] = MVector2(m_position.x + m_scale.x, m_position.y + m_scale.y);
	g_vertices[2] = MVector2(m_position.x + m_scale.x, m_position.y);

	render->setVertexPointer(M_FLOAT, 2, g_vertices);
	render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

void MGui2d::drawTexturedQuad(unsigned int textureId)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	MVector2 g_vertices[8];
	MVector2 g_texCoords[8];

	render->disableNormalArray();
	render->disableColorArray();
	render->enableVertexArray();
	render->enableTexCoordArray();

	g_vertices[0] = MVector2(m_position.x, m_position.y);
	g_vertices[1] = MVector2(m_position.x, m_position.y + m_scale.y);
	g_vertices[3] = MVector2(m_position.x + m_scale.x, m_position.y + m_scale.y);
	g_vertices[2] = MVector2(m_position.x + m_scale.x, m_position.y);

	g_texCoords[0] = MVector2(0, 0);
	g_texCoords[1] = MVector2(0, 1);
	g_texCoords[3] = MVector2(1, 1);
	g_texCoords[2] = MVector2(1, 0);

	render->setTexCoordPointer(M_FLOAT, 2, g_texCoords);
	render->setVertexPointer(M_FLOAT, 2, g_vertices);

	render->bindTexture(textureId);
	render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

MVector2 MGui2d::getMouseLocalPosition(void)
{
	MVector2 mousePosition;
	MMouse * mouse = MMouse::getInstance();

	if(m_parentWindow)
	{
		if(isScrollAffected())
		{
			mousePosition.set(
				mouse->getXPosition() - m_parentWindow->getPosition().x - m_parentWindow->getScroll().x, 
				mouse->getYPosition() - m_parentWindow->getPosition().y - m_parentWindow->getScroll().y
				);
		}
		else
		{
			mousePosition.set(
				mouse->getXPosition() - m_parentWindow->getPosition().x, 
				mouse->getYPosition() - m_parentWindow->getPosition().y
				);
		}
	}
	else{
		mousePosition.set((float)mouse->getXPosition(), (float)mouse->getYPosition());
	}

	return mousePosition;
}

bool MGui2d::isMouseInside(void)
{
	if(isPointInside(getMouseLocalPosition()))
		return true;

	return false;
}
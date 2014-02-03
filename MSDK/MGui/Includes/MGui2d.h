/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui2d.h
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


#ifndef _M_GUI2D_H
#define _M_GUI2D_H


class MGui2d
{
public:

	MGui2d(void);
	virtual ~MGui2d(void);

protected:

	// linked variable
	M_VAR_TYPE m_variableType;
	void * m_variablePointer;

	// is affected by scrolling
	bool m_isScrollAffect;

	// properties
	bool m_isVisible;
	bool m_isHighLight;
	bool m_isPressed;

	// font
	unsigned int m_fontId;

	// parent
	MGuiWindow * m_parentWindow;

	// position / scale
	MVector2 m_position;
	MVector2 m_scale;

	// text
	float m_textSize;

	// textures
	bool m_hasNormalTexture;
	bool m_hasPressedTexture;
	bool m_hasHighLightTexture;
	unsigned int m_normalTexture;
	unsigned int m_pressedTexture;
	unsigned int m_highLightTexture;

	// style
	bool m_isDrawingText;
	bool m_centerText;
	bool m_autoScaleFromText;

	// colors
	MVector4 m_normalColor;
	MVector4 m_pressedColor;
	MVector4 m_highLightColor;
	MVector4 m_textColor;

	// shadow textures
	bool m_hasShadow;

	// custom variable
	void * m_customVariable;

public:

	// position / scale
	virtual void setPosition(const MVector2 & position);
	virtual void setXPosition(float x);
	virtual void setYPosition(float y);
	virtual void setScale(const MVector2 & scale);
	virtual void setXScale(float x);
	virtual void setYScale(float y);

	// drawing
	virtual void drawTexturedQuad(unsigned int textureId);
	virtual void drawShadow(void);
	virtual void drawQuad(void);

	// scrolling affect
	virtual bool isScrollAffected(void){ return m_isScrollAffect; }
	virtual void setScrollAffect(bool affect){ m_isScrollAffect = affect; }

	// linked variable
	virtual M_VAR_TYPE getVariableType(void){ return m_variableType; }
	virtual void * getVariablePointer(void){ return m_variablePointer; }
	virtual void enableVariable(void * variable, M_VAR_TYPE type){ m_variablePointer = variable; m_variableType = type; updateFromVariable(); }
	virtual void disableVariable(void){ m_variablePointer = NULL; }
	virtual void updateFromVariable(void){}

	// shadows
	virtual void setShadow(bool shadow){ m_hasShadow = shadow; }
	virtual bool hasShadow(void){ return m_hasShadow; }

	// text
	virtual void setDrawingText(bool draw){ m_isDrawingText = draw; }
	virtual void setTextSize(float size){ m_textSize = size; autoScaleFromText(); }
	virtual bool isDrawingText(void){ return m_isDrawingText; }
	virtual float getTextSize(void){ return m_textSize; }
	inline void setCenterText(bool centerText){ m_centerText = centerText; }
	inline void setAutoScaleFromText(bool autoScaleFromText){ m_autoScaleFromText = autoScaleFromText; }
	inline bool isTextCentered(void){ return m_centerText; }
	inline bool isAutoScaleFromTextEnabled(void){ return m_autoScaleFromText; }
	
	// parent
	virtual void setParentWindow(MGuiWindow * window){ m_parentWindow = window; }
	virtual MGuiWindow * getParentWindow(void){ if(m_parentWindow) return m_parentWindow; return NULL; }

	// position / scale
	virtual MVector2 getPosition(void){ return m_position; }
	virtual MVector2 getScale(void){ return m_scale; }

	// colors
	virtual void setNormalColor(const MVector4 & color){ m_normalColor = color; }
	virtual void setPressedColor(const MVector4 & color){ m_pressedColor = color; }
	virtual void setHighLightColor(const MVector4 & color){ m_highLightColor = color; }
	virtual void setTextColor(const MVector4 & color){ m_textColor = color; }
	virtual MVector4 getNormalColor(void){ return m_normalColor; }
	virtual MVector4 getPressedColor(void){ return m_pressedColor; }
	virtual MVector4 getHighLightColor(void){ return m_highLightColor; }
	virtual MVector4 getTextColor(void){ return m_textColor; }

	// textures
	virtual void setNormalTexture(const char * filename);
	virtual void setPressedTexture(const char * filename);
	virtual void setHighLightTexture(const char * filename);
	virtual void disableNormalTexture(void){ m_hasNormalTexture = false; }
	virtual void disablePressedTexture(void){ m_hasPressedTexture = false; }
	virtual void disableHighLightTexture(void){ m_hasHighLightTexture = false; }
	virtual bool hasNormalTexture(void){ return m_hasNormalTexture; }
	virtual bool hasPressedTexture(void){ return m_hasPressedTexture; }
	virtual bool hasHighLightTexture(void){ return m_hasHighLightTexture; }
	virtual unsigned int getNormalTexture(void){ return m_normalTexture; }
	virtual unsigned int getPressedTexture(void){ return m_pressedTexture; }
	virtual unsigned int getHighLightTexture(void){ return m_highLightTexture; }

	// font
	virtual void setFontId(unsigned int fontId){ m_fontId = fontId; }
	virtual unsigned int getFontId(void){ return m_fontId; }
	virtual MGuiTextureFont * getFont(void);

	// point detection
	virtual bool isPointInside(const MVector2 & point);

	// mouse
	virtual bool isMouseInside(void);
	virtual MVector2 getMouseLocalPosition(void);

	// properties
	virtual bool isVisible(void){ return m_isVisible; }
	virtual bool isHighLight(void){ return m_isHighLight; }
	virtual bool isPressed(void){ return m_isPressed; }
	virtual void setVisible(bool visible){ m_isVisible = visible; }
	virtual void setPressed(bool pressed){ m_isPressed = pressed; }
	virtual void setHighLight(bool highLight){ m_isHighLight = highLight; }

	// drawing
	virtual void draw(void);

	// rescale
	virtual void rescale(void){}
	virtual void autoScaleFromText(void);

	virtual void onEvent(MWinEvent * windowEvent){}

	// get text
	virtual const char * getText(void){ return NULL; }

	// custom variable
	inline void * getCustomVariable(void){ return m_customVariable; }
	inline void setCustomVariable(void * customVariable){ m_customVariable = customVariable; }
};

#endif

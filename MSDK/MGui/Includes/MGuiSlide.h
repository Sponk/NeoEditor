/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiSlide.h
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


#ifndef _M_GUI_SLIDE_H
#define _M_GUI_SLIDE_H


class MGuiSlide : public MGui2d
{
private:

	float m_value;
	float m_minValue;
	float m_maxValue;

	MVector2 m_offset; // offset selection (for slide moving)

	MVector2 m_direction; // slide direction

	MGui2d m_button; // slide button

	// events
	void (* m_pointerEvent)(MGuiSlide * slide, MGuiEvent * guiEvent);

public:

	// linked variable
	void sendVariable(void);
	void updateFromVariable(void);

	// constructor
	MGuiSlide(MVector2 position, MVector2 scale, MVector4 color, void (* pointerEvent)(MGuiSlide * slide, MGuiEvent * guiEvent));

	// slide properties
	void setDirection(const MVector2 & direction);
	void setValue(float value);
	inline void setMinValue(float min){ m_minValue = min; if(m_value < min) m_value = min; }
	inline void setMaxValue(float max){ m_maxValue = max; if(m_value > max) m_value = max; }
	inline void setButtonScale(const MVector2 & scale){ m_button.setScale(scale); }

	float getNormalizedValue(void);
	inline float getValue(void){ return m_value; }
	inline float getMinValue(void){ return m_minValue; }
	inline float getMaxValue(void){ return m_maxValue; }
	float getValueFromPoint(const MVector2 & point);

	inline MVector2 getDirection(void){ return m_direction; }
	inline MVector2 getButtonScale(void){ return m_button.getScale(); }
	inline MVector2 getButtonPosition(void){ return m_button.getPosition(); }
	MVector2 getPointfromValue(float value);

	// on change
	void onChange(void);

	// update position
	void updatePosition(void);

	// events
	void onEvent(MWinEvent * windowEvent);
	inline void setPointerEvent(void (* pointerEvent)(MGuiSlide * slide, MGuiEvent * guiEvent)){ m_pointerEvent = pointerEvent; }

	void draw(void);
};

#endif

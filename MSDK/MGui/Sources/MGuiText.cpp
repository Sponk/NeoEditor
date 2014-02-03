/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiText.cpp
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


// constructor
MGuiText::MGuiText(const char * text, const MVector2 & position, float size, const MVector4 & color)
{
	setParentWindow(NULL);
	setText(text);
	setPosition(position);
	setTextSize(size);
	setNormalColor(color);
}

void MGuiText::draw(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	if(! isVisible())
		return;

	updateFromVariable();
	render->enableTexture();
	render->setColor4(getNormalColor());
	getFont()->draw(getText(), getPosition(), getTextSize());
}


void MGuiText::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;

	switch(getVariableType())
	{
	case M_VAR_BOOL:
		{
			bool * value = (bool *)getVariablePointer();

			if(*value)
				setText("1");
			else
				setText("0");
		}
		break;
	case M_VAR_INT:
		{
			int * value = (int *)getVariablePointer();

			char text[256];
			sprintf(text, "%d", *value);

			setText(text);
		}
		break;
	case M_VAR_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();

			char text[256];
			sprintf(text, "%d", *value);

			setText(text);
		}
		break;
	case M_VAR_FLOAT:
		{
			float * value = (float *)getVariablePointer();

			char text[256];
			sprintf(text, "%0.2f", *value);

			setText(text);
		}
		break;
	case M_VAR_STRING:
		{
			MString * value = (MString *)getVariablePointer();
			setText(value->getSafeString());
		}
		break;
	}
}

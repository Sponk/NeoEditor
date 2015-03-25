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


#ifndef __OTEXT_H
#define __OTEXT_H

namespace Neo
{
// align modes
enum TEXT_ALIGN_MODES
{
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_CENTER
};

// MOText
class M_ENGINE_EXPORT OText : public Object3d
{
public:

	// constructor / destructor
	OText(FontRef * fontRef);
	~OText(void);

	// copy constructor
	OText(const OText & text);

private:

	// font ref
	FontRef * m_fontRef;

	// text
	String m_text;

	// align
	TEXT_ALIGN_MODES m_align;
	vector <float> m_linesOffset;

	// size
	float m_size;

	// color
	Vector4 m_color;

	// bounding box
	Box3d m_boundingBox;

private:

	void updateLinesOffset(void);
	void prepare(void);

public:

	// type
	int getType(void){ return M_OBJECT3D_TEXT; }

	// font
	void setFontRef(FontRef * fontRef);
	Font * getFont(void);
	inline FontRef * getFontRef(void){ return m_fontRef; }

	// text
	void setText(const char * text);
	inline const char * getText(void){ return m_text.getSafeString(); }

	// lines offset
	inline vector <float> * getLinesOffset(void){ return &m_linesOffset; }
	
	// align
	void setAlign(TEXT_ALIGN_MODES align);
	inline TEXT_ALIGN_MODES getAlign(void){ return m_align; }

	// size
	void setSize(float size);
	inline float getSize(void){ return m_size; }

	// color
	inline void setColor(const Vector4 & color){ m_color = color; }
	inline Vector4 getColor(void) const { return m_color; }

	// bounding box
	inline Box3d * getBoundingBox(void){ return &m_boundingBox; }

	// visibility
	void updateVisibility(OCamera * camera);
};
}
#endif

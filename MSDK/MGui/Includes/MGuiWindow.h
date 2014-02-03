/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiWindow.h
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


#ifndef _M_GUI_WINDOW_H
#define _M_GUI_WINDOW_H


class MGuiWindow : public MGui2d
{
private:

	bool m_breakEvents;
	bool m_isScrolled;

	// scrolling
	MVector2 m_scroll;
	MVector2 m_maxScroll, m_minScroll;

	// margin (for scrolling)
	MVector2 m_margin;

	// slides for scrolling
	MGuiSlide m_hScrollSlide; // horizontal slide
	MGuiSlide m_vScrollSlide; // vertical slide

	// events function pointer
	void (* m_pointerEvent)(MGuiWindow * window, MGuiEvent * guiEvent);

private:

	std::vector <MGui2d *>			m_objects;

	std::vector <MGuiButton *>		m_buttons;
	std::vector <MGuiMenu *>		m_menus;
	std::vector <MGuiText *>		m_texts;
	std::vector <MGuiImage *>		m_images;
	std::vector <MGuiEditText *>	m_editTexts;
	std::vector <MGuiSlide *>		m_slides;

public:

	// constructor / destructor
	MGuiWindow(const MVector2 & position, const MVector2 & scale, const MVector4 & color, void (* pointerEvent)(MGuiWindow * window, MGuiEvent * guiEvent));
	virtual ~MGuiWindow(void);

private:

	// scrolling
	bool isHorizontalScroll(void);
	bool isVerticalScroll(void);
	void rescaleScrollingBar(void);

	// internal events
	void internalEvent(MWinEvent * windowEvent);

public:

	// clear
	void clear(void);

	// scroll bar
	bool isScrollBarPressed(void);
	void updateScrollingBar(void);
	void setScrollBarVisible(bool visible){ m_hScrollSlide.setVisible(visible); m_vScrollSlide.setVisible(visible); }
	
	// auto scale
	void autoScale(void);

	// margin
	inline void setMargin(const MVector2 & margin){ m_margin = margin; }
	inline MVector2 getMargin(void){ return m_margin; }

	// scrolling
	inline bool isScrolled(void){ return m_isScrolled; }
	inline void setScrolled(bool scroll){ m_isScrolled = scroll; }
	void resizeScroll(void);
	void moveScroll(const MVector2 & direction);
	void limitScroll(void);

	inline MVector2 getScroll(void){ return m_scroll; }
	inline void setScroll(const MVector2 & scroll){ m_scroll = scroll; }

	void drawWindowMenus(void);

	int onWindowMenusEvent(MWinEvent * windowEvent);

	// get
	inline unsigned int getButtonsNumber(void){ return m_buttons.size(); }
	inline unsigned int getMenusNumber(void){ return m_menus.size(); }
	inline unsigned int getTextsNumber(void){ return m_texts.size(); }
	inline unsigned int getImagesNumber(void){ return m_images.size(); }
	inline unsigned int getEditTextsNumber(void){ return m_editTexts.size(); }
	inline unsigned int getSlidesNumber(void){ return m_slides.size(); }

	inline MGuiButton * getButton(const unsigned int id){ return m_buttons[id]; }
	inline MGuiMenu * getMenu(const unsigned int id){ return m_menus[id]; }
	inline MGuiText * getText(const unsigned int id){ return m_texts[id]; }
	inline MGuiImage * getImage(const unsigned int id){ return m_images[id]; }
	inline MGuiEditText * getEditText(const unsigned int id){ return m_editTexts[id]; }
	inline MGuiSlide * getSlide(const unsigned int id){ return m_slides[id]; }

	// add
	void addButton(MGuiButton * button);
	void addMenu(MGuiMenu * menu);
	void addText(MGuiText * text);
	void addImage(MGuiImage * image);
	void addEditText(MGuiEditText * editText);
	void addSlide(MGuiSlide * slide);

	// break events
	inline bool breakEvents(void){ return m_breakEvents; }

	// editing
	bool isSomethingEditing(void);

	// events
	void onEvent(MWinEvent * windowEvent);
	void setPointerEvent(void (*pointerEvent)(MGuiWindow * window, MGuiEvent * guiEvent)){ m_pointerEvent = pointerEvent; }

	void setHighLight(bool highLight);
	void draw(void);
};

#endif

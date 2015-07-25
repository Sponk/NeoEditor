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


#ifndef __INPUT_CONTEXT
#define __INPUT_CONTEXT

namespace Neo
{
// multitouch
enum TOUCH_PHASE
{
	TOUCH_BEGIN = 0,
	TOUCH_UPDATE,
	TOUCH_END,
	TOUCH_CANCELLED,
	TOUCH_NONE
};


class NEO_CORE_EXPORT InputContext
{
public:
    
	virtual ~InputContext(void){}

	// create inputs
	virtual void createKey(const char * name) = 0;
	virtual void createAxis(const char * name, bool flush=false) = 0;
	virtual void createProperty(const char * name) = 0;

	// inputs events
	virtual void downKey(const char * name) = 0;
	virtual void upKey(const char * name) = 0;
	virtual void setAxis(const char * name, float axis) = 0;
	virtual void setProperty(const char * name, int prop) = 0;

	virtual bool isKeyPressed(const char * name) = 0;
	virtual bool onKeyDown(const char * name) = 0;
	virtual bool onKeyUp(const char * name) = 0;
	virtual float getAxis(const char * name) = 0;
	virtual int getProperty(const char * name) = 0;
    
    // touch events
    virtual void beginTouch(int touchID, Vector2 touchPoint) = 0;
    virtual void updateTouch(int touchID, Vector2 touchPoint) = 0;
    virtual void endTouch(int touchID, Vector2 touchPoint) = 0;
    virtual void cancelTouch(int touchID, Vector2 touchPoint) = 0;
	
    virtual Vector2 getTouchPosition(int touchID) = 0;
    virtual Vector2 getLastTouchPosition(int touchID) = 0;
    virtual TOUCH_PHASE getTouchPhase(int touchID) = 0;
    
	// flush
	virtual void flush(void) = 0;

	virtual void setLastChar(unsigned int c) = 0;
	virtual unsigned int popLastChar() = 0;
};
}
#endif

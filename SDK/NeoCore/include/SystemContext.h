//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __SYSTEM_CONTEXT_H
#define __SYSTEM_CONTEXT_H

namespace Neo
{
/**
 * @brief This class implements a basic interface to the underlying operating system.
 *
 * It provides functionality to accomplish environment dependant functionality like changing the
 * screen resolution (or window size), show and hide the mouse cursor and similar.
 *
 * This class usually only has one instance that can be fetched from NeoEngine with NeoEngine::getSystemContext().
 */
class NEO_CORE_EXPORT SystemContext
{
public:

	// destructor
	virtual ~SystemContext(void){}

	/**
	 * @brief Fetches the current screen resolution or window size in pixels.
	 *
	 * Needs to be used like this:
	 * \code
	 * unsigned int w, h;
	 * system->getScreenSize(&w, &h);
	 * \endcode
	 *
	 * @param width A pointer to an unsigned integer variable.
	 * @param height A pointer to an unsigned integer variable.
	 */
	virtual void getScreenSize(unsigned int * width, unsigned int * height) = 0;

	/**
	 * @brief Returns the current screen resolution or window size in pixels.
	 * @return A Vector2 with the current window size.
	 */
	virtual Vector2 getScreenSize() = 0;

	/**
	 * @brief Changes the current cursor position.
	 * @param x X-Coordinate in pixels
	 * @param y Y-Coordinate in pixels
	 */
	virtual void setCursorPosition(int x, int y){};

	/**
	 * @brief Hides the cursor.
	 */
	virtual void hideCursor(void){};

	/**
	 * @brief Shows the cursor.
	 */
	virtual void showCursor(void){};
	
	/**
	 * @brief Returns the working directory as a C string.
	 * @return The working directory path.
	 */
	virtual const char * getWorkingDirectory(void) = 0;

	/**
	 * @brief Returns the current system time in millis.
	 * @return The current system tick.
	 */
	virtual unsigned long getSystemTick(void) = 0;

	/**
	 * @brief Changes the window title of the game window.
	 * @param title The new title that shall be used.
	 */
	virtual void setWindowTitle(const char* title) = 0;
	
	/**
	 * @brief Changes the screen resolution or the window size depending
	 * on if the window is fullscreen.
	 * 
	 * @param width The new width in pixels.
	 * @param height The new height in pixels.
	 */
	virtual void setScreenSize(unsigned int width, unsigned int height) = 0;
	virtual void sleep(long int millis) = 0;
};

class NEO_CORE_EXPORT SystemContextDummy : public SystemContext
{
	Vector2 m_screensize;
public:
	virtual void getScreenSize(unsigned int* width, unsigned int* height) { *width = m_screensize.x; *height = m_screensize.y; }
	virtual Vector2 getScreenSize() { return m_screensize; }

	virtual const char* getWorkingDirectory(void) { return "./"; }
	virtual unsigned long getSystemTick(void) { return 0; }
	virtual void setWindowTitle(const char* title) {}

	virtual void setScreenSize(unsigned int width, unsigned int height) { m_screensize = Vector2(width, height); }
	virtual void sleep(long int millis) {}
};

}
#endif

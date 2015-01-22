/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MKeyboard.h
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


#ifndef _M_KEYBOARD_H
#define _M_KEYBOARD_H


class MKeyboard
{
protected:

	bool m_key[256];

public:

	// instance
	static MKeyboard * getInstance()
	{
		static MKeyboard m_instance;
		return &m_instance;
	};

	inline void onKeyDown(const unsigned int id){ m_key[id] = true;  }
	inline void onKeyUp(const unsigned int id){ m_key[id] = false; }

	inline bool isKeyPressed(const unsigned int id){ return m_key[id]; }
};

#endif
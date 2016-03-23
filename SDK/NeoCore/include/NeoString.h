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

#ifndef __STRING_H
#define __STRING_H

namespace Neo
{

#ifdef SWIG_JAVA
%rename String NeoString;
#endif

/**
 * @brief Implements a framework for string manipulation that
 * operates directly on C strings.
 */
class NEO_CORE_EXPORT String
{
public:
	
	String(void);
	String(const char* data);

#ifndef SWIG_JAVA
	String(const String& string);
#endif
	~String(void);
	
private:
	
	char * m_data;
	
public:
	
	/**
	 * @brief Clears the string.
	 */
	void clear(void);
	
	/**
	 * @brief Copies the given string to the internal buffer.
	 * @param data The new string data.
	 */
	void set(const char * data);
	
	/**
	 * @brief Returns the C string.
	 * This method is ensured to return a valid C string and never NULL.
	 * @return The C string
	 */
	const char * getSafeString(void);
	
	/**
	 * @brief Returns the current internal string buffer.
	 * Attention: Might return NULL!
	 * 
	 * @return The internal string buffer
	 */
	inline const char * getData(void){ return m_data; }
	
	inline void operator = (const String & string){ set(string.m_data); }
	inline void operator = (const char * data){ set(data); }
};
}
#endif 

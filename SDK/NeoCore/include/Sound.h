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


#ifndef __SOUND_H
#define __SOUND_H

namespace Neo
{
class NEO_CORE_EXPORT Sound
{
public:

	Sound(void);
	~Sound(void);

private:

	void * m_data;
	SOUND_FORMAT m_format;

	unsigned int m_sampleRate;
	unsigned int m_size;

public:

	void create(SOUND_FORMAT format, unsigned int size, unsigned int sampleRate);

	inline void * getData(void){ return m_data; }
	inline SOUND_FORMAT getFormat(void){ return m_format; }
	inline unsigned int getSampleRate(void){ return m_sampleRate; }
	inline unsigned int getSize(void){ return m_size; }
};
}
#endif

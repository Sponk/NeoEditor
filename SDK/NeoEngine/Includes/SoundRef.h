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


#ifndef __SOUND_REF_H
#define __SOUND_REF_H

namespace Neo
{
// Sound ref
class NEO_ENGINE_EXPORT SoundRef : public DataRef
{
private:

	// data
	unsigned int m_bufferId;

public:

	// constructor / destructor
	SoundRef(unsigned int bufferId, const char * filename);
	~SoundRef(void);

	// clear / destroy
	void clear(void);
	void destroy(void);

	// get new
	static SoundRef * getNew(unsigned int bufferId, const char * filename);

public:

	// update
	void update(void);

	// type
	int getType(void){ return M_REF_SOUND; }

	// data
	inline void setBufferId(unsigned int bufferId){ m_bufferId = bufferId; }
	inline unsigned int getBufferId(void){ return m_bufferId; }
};
}
#endif

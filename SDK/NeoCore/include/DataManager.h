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


#ifndef __DATA_MANAGER_H
#define __DATA_MANAGER_H

namespace Neo
{
// Data ref
class NEO_CORE_EXPORT DataRef
{
protected:

	long m_time;
	String m_filename;
	unsigned int m_score;

public:

	// constructor
	DataRef(void);

	// clear / destroy
	virtual void clear(void) = 0;
	virtual void destroy(void) = 0;

public:

	// score
	void incrScore(void);
	void decrScore(void);
	inline unsigned int getScore(void){ return m_score; }

	// update
	virtual void update(void) = 0;

	// type
	virtual int getType(void) = 0;

	// time
	inline void setTime(long time){ m_time = time; }
	inline long getTime(void){ return m_time; }

	// filename
	inline const char * getFilename(void){ return m_filename.getData(); }
};


// Data manager
class NEO_CORE_EXPORT DataManager
{
private:

	// refs
	vector <DataRef *> m_refs;

public:

	// destructor
	virtual ~DataManager(void);

public:

	// clear
	virtual void clear(void);

	// refs
	void addRef(DataRef * ref);
	inline DataRef * getRef(unsigned int id){ return m_refs[id]; }
	inline unsigned int getRefsNumber(void){ return m_refs.size(); }

	/**
	 * @brief Updates all references by re-loading them from disk.
	 *
	 * If the file belonging to a DataRef was changed on disk since it was loaded
	 * or last updated it will be reloaded and the time stamp updated.
	 */
	void update();
};
}
#endif

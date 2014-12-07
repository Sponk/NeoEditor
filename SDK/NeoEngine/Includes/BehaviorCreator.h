/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MBehaviorCreator.h
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


#ifndef _M_BEHAVIOR_CREATOR_H
#define _M_BEHAVIOR_CREATOR_H

namespace Neo
{
/**
 * @brief This class is used to instanciate new behavior objects.
 */
class M_ENGINE_EXPORT BehaviorCreator
{
public:

	BehaviorCreator(const char * name, int objectFilter, Behavior * (*getNewBehaviorFunctionPointer)(Object3d * parentObject));
	virtual ~BehaviorCreator(void){}

private:

	string m_name;

	int m_objectFilter;

	Behavior * (*m_getNewBehavior)(Object3d * parentObject);

public:

	// filter
	inline int getObjectFilter(void){ return m_objectFilter; }

	// name
	inline void setName(const char * name){ m_name = name; }
	inline const char * getName(void){ return m_name.c_str(); }

	// new behavior
	Behavior * getNewBehavior(Object3d * parentObject);
};
}
#endif

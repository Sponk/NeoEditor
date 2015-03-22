/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBFollow.h
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


#ifndef _FOLLOW_BEHAVIOR_H
#define _FOLLOW_BEHAVIOR_H

namespace Neo
{
/**
 * The FollowBehavior class implements a behavior that forces the parent object
 * to follow the target object.
 *
 * The method used is either physics based if the
 * parent is a physically active object or translation based if it is not.
 * @see Neo::Behavior
 */
class FollowBehavior : public Behavior
{
public:

	// constructors / destructors
    FollowBehavior(Object3d * parentObject);
    FollowBehavior(FollowBehavior & behavior, Object3d * parentObject);
    ~FollowBehavior(void);

private:

	// variables
	bool m_local;
	float m_delay;
	Vector3 m_offset;
	String m_targetName;
	
public:

	// destroy
	void destroy(void);

	// get new
	static Behavior * getNew(Object3d * parentObject);

	// get copy
	Behavior * getCopy(Object3d * parentObject);

	// name
	static const char * getStaticName(void){ return "Follow"; }
	const char * getName(void){ return getStaticName(); }

	// events
	void update(void);
	void runEvent(int param){}

	// variables
	unsigned int getVariablesNumber(void);
	NeoVariable getVariable(unsigned int id);
};
}
#endif

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef _B_LOOK_AT_H
#define _B_LOOK_AT_H

#include <NeoEngine.h>

namespace Neo
{
class LookAtBehavior : public Neo::Behavior
{
public:

	// constructors / destructors
    LookAtBehavior(Object3d * parentObject);
    LookAtBehavior(LookAtBehavior & behavior, Object3d * parentObject);
    ~LookAtBehavior(void);

private:

	// variables
	String m_targetName;

public:

	// destroy
	void destroy(void);

	// get new
	static Behavior * getNew(Object3d * parentObject);

	// get copy
	Behavior * getCopy(Object3d * parentObject);

	// name
	static const char * getStaticName(void){ return "LookAt"; }
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

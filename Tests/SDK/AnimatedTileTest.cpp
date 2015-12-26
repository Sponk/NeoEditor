//========================================================================
// Copyright (c) 2015 Yannick Pflanzer <neo-engine.de>
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

#include <gtest/gtest.h>
#include <AnimatedTile.h>

using namespace std;
using namespace Neo2D;

TEST(AnimatedTileTest, UpdateAnimation_test)
{
	Animation anim(1, 0, 4, 1000);

	// Test if timer works
	ASSERT_EQ(1, anim.getNextFrame(0.1));
	ASSERT_EQ(2, anim.getNextFrame(1));

	// Frame == 3
	anim.getNextFrame(1);

	// Test if loop works
	ASSERT_EQ(0, anim.getNextFrame(1));
}
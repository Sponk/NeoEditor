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
#include <CulledSpriteBatch.h>

using namespace std;
using namespace Neo2D;

TEST(CulledSpriteBatchTest, UpdateVisibility_culled_away_test)
{
	CulledSpriteBatch csb;
	Widget w;
	Canvas c;

	w.setSize(100, 100);
	c.setSize(100, 100);
	csb.addSprite(&w);

	csb.updateVisibility(&c);
	ASSERT_TRUE(w.isVisible());
}

TEST(CulledSpriteBatchTest, UpdateVisibility_is_visible_test)
{
	CulledSpriteBatch csb;
	Widget w;
	Canvas c;

	w.setPosition(Vector2(-101, -101));
	w.setSize(100, 100);
	c.setSize(100, 100);
	csb.addSprite(&w);

	csb.updateVisibility(&c);
	ASSERT_FALSE(w.isVisible());
}

TEST(CulledSpriteBatchTest, UpdateVisibility_stress_test)
{
	CulledSpriteBatch csb;
	Widget w[512];
	Canvas c;

	c.setSize(100, 100);
	for(int i = 0; i < 512; i++)
		csb.addSprite(&w[i]);

	csb.updateVisibility(&c);
}
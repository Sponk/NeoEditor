//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <neo-engine.de>
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
#include <NeoCore.h>
#include <Variable.h>
#include <cstring>

using namespace Neo;

class TestCore: public testing::Test
{
public:

	void SetUp()
	{
		
	}

	void TearDown()
	{
		
	}
};

TEST_F(TestCore, Variable_test)
{
	Variable<int> intvar(150, VARIABLE_INT);

	EXPECT_EQ(VARIABLE_INT, intvar.getType());
	EXPECT_EQ(150, intvar.getData());

	Variable<const char*> str("This is a string!!!", VARIABLE_STRING);
	EXPECT_EQ(VARIABLE_STRING, str.getType());
	EXPECT_EQ(0, strcmp(str.getData(), "This is a string!!!"));

	Variable<float>* var = new Variable<float>(200.0f, VARIABLE_FLOAT);
	var->setData(var->getData() + 50.0f);
	
	EXPECT_EQ(VARIABLE_FLOAT, var->getType());
	EXPECT_EQ(250.0f, var->getData());
	
	delete var;
}

TEST_F(TestCore, String_test)
{
	String s;
	ASSERT_EQ(NULL, s.getData());
	EXPECT_TRUE(NULL != s.getSafeString());

	// Test setting the string
	s.set("TestString");
	ASSERT_EQ(0, strcmp(s.getData(), "TestString"));

	s.set("TestString2");
	ASSERT_EQ(0, strcmp(s.getSafeString(), "TestString2"));

	// Test clearing
	s.clear();
	ASSERT_EQ(NULL, s.getData());
	EXPECT_TRUE(NULL != s.getSafeString());

}

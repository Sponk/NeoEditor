#include <gtest/gtest.h>
#include <NeoEngine.h>

TEST(ConfigurationRegistryTest, AddVariable)
{
	Neo::ConfigurationRegistry reg;
	EXPECT_NE(nullptr, reg.registerVariable("test"));
}

TEST(ConfigurationRegistryTest, SetVariableNoCallback)
{
	Neo::ConfigurationRegistry reg;
	std::string& var = *reg.registerVariable("test");

	reg.setVariable("test", "Hello World");
	EXPECT_EQ("Hello World", var);
}

TEST(ConfigurationRegistryTest, SetVariableCallback)
{
	Neo::ConfigurationRegistry reg;
	int counter = 0;

	std::string& var = *reg.registerVariable("test", [&counter](std::string& str) { counter++; });
	reg.setVariable("test", "Hello World");

	EXPECT_EQ(1, counter);
}

TEST(ConfigurationRegistryTest, SetNonExisting)
{
	Neo::ConfigurationRegistry reg;
	reg.setVariable("test", "Hello World");
}

TEST(ConfigurationRegistryTest, LoadExisting)
{
	Neo::ConfigurationRegistry reg;
	std::ofstream out("registry_test.cfg");
	
	ASSERT_FALSE(!out);
	
	out << "(config (testvar \"abc\"))" << std::endl;
	out.close();
	
	ASSERT_TRUE(reg.load("registry_test.cfg"));
	EXPECT_EQ("abc", *reg.getVariable("testvar"));
}

TEST(ConfigurationRegistryTest, LoadNonExisting)
{
	Neo::ConfigurationRegistry reg;
	ASSERT_FALSE(reg.load("registry_test_doesntexist.cfg"));
}

TEST(ConfigurationRegistryTest, SaveExisting)
{
	Neo::ConfigurationRegistry reg;
	
	*reg.getVariable("testvar") = "value";
	
	ASSERT_TRUE(reg.save("testvar_test.cfg"));
	
	{
		char* content = readTextFile("testvar_test.cfg");
		ASSERT_TRUE(content != nullptr);
		
		EXPECT_STREQ("(config\n\t(testvar \"value\")\n)\n", content);
		delete[] content;
	}
}

TEST(ConfigurationRegistryTest, SaveNonExisting)
{
	Neo::ConfigurationRegistry reg;
	ASSERT_FALSE(reg.save("somefolder/doesntexist/empty.cfg"));
}

TEST(ConfigurationRegistryTest, SaveLoad)
{
	// Save a config
	{
		Neo::ConfigurationRegistry reg;
		*reg.getVariable("testvar") = "value";
		ASSERT_TRUE(reg.save("saveload_test.cfg"));
	}
	
	// Load a config
	{
		Neo::ConfigurationRegistry reg;
		ASSERT_TRUE(reg.load("saveload_test.cfg"));
		
		EXPECT_EQ("value", *reg.getVariable("testvar"));
	}
}

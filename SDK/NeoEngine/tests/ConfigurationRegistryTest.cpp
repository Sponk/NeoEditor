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

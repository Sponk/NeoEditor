#include <gtest/gtest.h>
#include <Translator.h>
#include <fstream>

using namespace Neo2D;
using namespace Gui;

class TranslatorTest: public ::testing::Test
{
protected:
	virtual void TearDown()
	{
		Translator::clear();
	}
};

std::string generateTranslationFile()
{
	std::string file("test_translation.csv");
	std::ofstream out(file);

	if(!out)
		return "";

	out << "Input\tOutput";
	out.close();

	return file;
}

TEST_F(TranslatorTest, TranslateNothingFound)
{
	EXPECT_STREQ("Input", Translator::translate("Input"));
}

TEST_F(TranslatorTest, TranslateFound)
{
	ASSERT_TRUE(Translator::loadTranslation(generateTranslationFile().c_str()));
	ASSERT_STREQ("Output", Translator::translate("Input"));
}

TEST_F(TranslatorTest, LoadTranslation)
{
	ASSERT_TRUE(Translator::loadTranslation(generateTranslationFile().c_str()));
}

TEST_F(TranslatorTest, LoadNonExistingTranslation)
{
	EXPECT_FALSE(Translator::loadTranslation("DoesNotExist"));
	EXPECT_STREQ("Input", Translator::translate("Input"));
}
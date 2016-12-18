#include <gtest/gtest.h>
#include <fstream>
#include "../Project.h"

using namespace std;

TEST(ProjectTest, LoadNonExistingFile)
{
	Project p;
	EXPECT_FALSE(p.load("DoesNotExist"));
}

TEST(ProjectTest, LoadCorruptFile_no_content)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_empty_project)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project)";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_startlevel)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (author \"\") (version \"\") (neo-version \"\") (assets \"\") (description \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_name)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (start-level \"\") (author \"\") (version \"\") (neo-version \"\") (assets \"\") (description \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_author)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (start-level \"\") (version \"\") (neo-version \"\") (assets \"\") (description \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_version)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (author \"\") (start-level \"\") (neo-version \"\") (assets \"\") (description \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_neo_version)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (author \"\") (version \"\") (start-level \"\") (assets \"\") (description \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_assets)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (author \"\") (version \"\") (neo-version \"\") (start-level \"\") (description \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_description)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (author \"\") (version \"\") (neo-version \"\") (assets \"\") (start-level \"\") (plugins \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, LoadCorruptFile_no_plugins)
{
	// Create test data
	{
		ofstream out("broken_config.txt");
		ASSERT_FALSE(!out);

		out << "(project (name \"\") (author \"\") (version \"\") (neo-version \"\") (assets \"\") (description \"\") (start-level \"\"))";
		out.close();
	}

	Project p;
	EXPECT_FALSE(p.load("broken_config.txt"));
}

TEST(ProjectTest, SaveInvalidFile)
{
	Project p;
	EXPECT_FALSE(p.save("DoesNotExist/project.nproj"));
}

TEST(ProjectTest, SaveLoadTest)
{
	// Create testdata
	{
		Project p("Name", "Description", "Version", "Author", "Start", "Assets", "NeoVersion", "BinaryDir");

		p.getPlugins().push_back("SomePlugin");
		p.getPlugins().push_back("SomeOtherPlugin");

		ASSERT_TRUE(p.save("TestProject.nproj"));
	}

	{
		Project p;
		ASSERT_TRUE(p.load("TestProject.nproj"));

		EXPECT_EQ("Name", p.getName());
		EXPECT_EQ("Description", p.getDescription());
		EXPECT_EQ("Author", p.getAuthor());
		EXPECT_EQ("Start", p.getLevel());
		EXPECT_EQ("Assets", p.getAssetDirectory());
		EXPECT_EQ("NeoVersion", p.getNeoVersion());
		EXPECT_EQ("Version", p.getVersion());

		ASSERT_EQ(2, p.getPlugins().size());
		EXPECT_EQ("SomePlugin", p.getPlugins()[0]);
		EXPECT_EQ("SomeOtherPlugin", p.getPlugins()[1]);
		EXPECT_EQ("BinaryDir", p.getBinaryDirectory());
	}
}

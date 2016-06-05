#include <gtest/gtest.h>
#include "../src/ProjectBackend.h"
#include <NeoCore.h>

TEST(ProjectBackendTest, ImportTemplateTest)
{
	createDirectory("test_template/assets", true);
	Project p("Test Project Template", "", "0.0", "Me", "scripts/lala.lua", "test_assets", "0.5");
	p.save("test_template/test_template.nproj");

	ProjectBackend b("./");
	b.importTemplate("test_template/test_template.nproj");

	ASSERT_EQ(1, b.getTemplates().size());

	// Make sure the path is right
	EXPECT_EQ("test_template/test_template.nproj", b.getTemplates().begin()->second.getFilePath());

	// Make sure the template name is right
	EXPECT_EQ("Test Project Template", b.getTemplates().begin()->first);
}

TEST(ProjectBackendTest, CreateProjectTest)
{
	// Prepare test
	{
		createDirectory("created_project");
		createDirectory("test_template/assets", true);
		Project p("Test Project Template", "", "0.0", "Me", "scripts/lala.lua", "test_assets", "0.5");
		p.save("test_template/test_template.nproj");
	}

	ProjectBackend b("./");
	b.importTemplate("test_template/test_template.nproj");

	b.createProject("My Project", "created_project", "Test Project Template");

	EXPECT_TRUE(isFileExist("created_project/assets"));
	EXPECT_TRUE(isFileExist("created_project/My Project.nproj"));

	Project p;
	ASSERT_TRUE(p.load("created_project/My Project.nproj"));
	EXPECT_EQ("My Project", p.getName());
}

TEST(ProjectBackendTest, RemoveProjectTest)
{
	ProjectBackend b("./");
	b.getProjects().push_back(Project());

	// Test
	b.removeProject(0, false);
	EXPECT_EQ(0, b.getProjects().size());
}

TEST(ProjectBackendTest, RemoveProjectTest_erase_files)
{
	// Generate test data
	Project p("Test Project", "", "0.0", "Me", "scripts/lala.lua", "test_assets", "0.5");
	p.save("test_project.nproj");

	createDirectory("test_assets");

	ProjectBackend b("./");
	b.getProjects().push_back(p);

	// Test
	EXPECT_TRUE(isFileExist("test_project.nproj"));

	b.removeProject(0, true);
	EXPECT_EQ(0, b.getProjects().size());
	EXPECT_FALSE(isFileExist("test_project.nproj"));
	EXPECT_FALSE(isFileExist("test_assets"));
}

TEST(ProjectBackendTest, ImportProjectTest)
{
	// Generate test data
	Project p("Test Project", "", "0.0", "Me", "scripts/lala.lua", "assets", "0.5");
	p.save("import_project.nproj");

	// Test
	{
		ProjectBackend b("./");
		size_t result = b.importProject("import_project.nproj");

		EXPECT_EQ(0, result);
		EXPECT_EQ(1, b.getProjects().size());
	}
}

TEST(ProjectBackendTest, SaveLoadConfigTest)
{
	// Save
	{
		ProjectBackend b("./");

		Project p("Test Project", "", "0.0", "Me", "scripts/lala.lua", "assets", "0.5");
		p.save("test_project.nproj");

		Project t = Project("TEST", "", "0.0", "Me", "scripts/lala.lua", "assets", "0.5");
		t.save("test_template.nproj");

		b.getProjects().push_back(p);
		b.getTemplates()["TEST"] = t;
		b.saveConfig("project_backend.cfg");
		ASSERT_TRUE(isFileExist("project_backend.cfg"));
	}

	// Load
	{
		ProjectBackend b("./");
		b.loadConfig("project_backend.cfg");
		EXPECT_EQ(1, b.getProjects().size());
		EXPECT_EQ(1, b.getTemplates().size());
	}
}
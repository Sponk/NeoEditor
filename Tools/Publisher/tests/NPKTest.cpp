#include <gtest/gtest.h>
#include "../src/NPKPublisher.h"
#include <Project.h>

using namespace Publish;

/*
 * bool Publish::NPKPublisher::publish(const char* projectFile,
									const char* executable,
									const char* output,
									bool verbose,
									const char* levelOverride,
									std::function<void(int)> progressCallback)
 */



class NPKTest: public ::testing::Test
{
public:

protected:
	virtual void SetUp()
	{
		/*
		 * const char* name, const char* description, const char* version, const char* author,
			const char* start, const char* assets, const char* neoversion) :
		 */
		Project p("testproject", "", "0.1", "me", "assets/levels/test.level", "assets", "0.5");
		p.getPlugins().push_back("NPKPlugin");
		p.getPlugins().push_back("SDLGame");

		p.save("publishtest.nproj");
	}
};

TEST_F(NPKTest, PublishNonExistingProject)
{
	NPKPublisher p;
	EXPECT_FALSE(p.publish("/DOESNOTEXIST", "LALALA", "TEST"));
}

TEST_F(NPKTest, PublishNonExistingPlayer)
{
	NPKPublisher p;
	EXPECT_FALSE(p.publish("testproject.nproj", "LALALA", "TEST"));
}

TEST_F(NPKTest, PublishNonAvailableOutput)
{
	NPKPublisher p;
	EXPECT_FALSE(p.publish("testproject.nproj", "NeoPlayer2", "/TEST"));
}

TEST_F(NPKTest, PublishSuccess)
{
	NPKPublisher p;
	EXPECT_TRUE(p.publish("publishtest.nproj", "./NeoPlayer2", "testproject_published"));
}
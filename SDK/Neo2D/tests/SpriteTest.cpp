#include <gtest/gtest.h>
#include <Sprite.h>
#include <NeoEngine.h>

using namespace Neo2D;

bool checkExistingLoader(const char* filename, Neo::Image* image)
{
	image->create(Neo::VAR_BYTE, 100, 100, 3);
	return true;
}

class SpriteTest: public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		Neo::NeoEngine::getInstance()->getImageLoader()->addLoader(checkExistingLoader);
	}

	virtual void TearDown()
	{
		Neo::NeoEngine::getInstance()->getImageLoader()->clear();
	}
};

TEST_F(SpriteTest, Load)
{
	Sprite s(0,0,0,0, "doesntmatter", nullptr);
	EXPECT_NE(0, s.getSize().x);
	EXPECT_NE(0, s.getSize().y);
}

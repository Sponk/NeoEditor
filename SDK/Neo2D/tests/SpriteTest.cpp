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
	Neo::Level level;
protected:
	virtual void SetUp()
	{
		Neo::NeoEngine::getInstance()->setLevel(&level);
		Neo::NeoEngine::getInstance()->getImageLoader()->addLoader(checkExistingLoader);
	}

	virtual void TearDown()
	{
		Neo::NeoEngine::getInstance()->getImageLoader()->clear();
		Neo::NeoEngine::getInstance()->setLevel(nullptr);
	}
};

TEST_F(SpriteTest, Load)
{
	Sprite s(0,0,0,0, "doesntmatter", nullptr);
	EXPECT_NE(0, s.getSize().x);
	EXPECT_NE(0, s.getSize().y);
}

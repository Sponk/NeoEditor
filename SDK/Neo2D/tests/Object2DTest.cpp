#include <gtest/gtest.h>
#include <Object2D.h>
#include <Container.h>

TEST(Object2DTest, Contains)
{
	Neo2D::Object2D object(0, 0, 10, 10, nullptr);
	ASSERT_FALSE(object.contains(Neo::Vector2(11, 11)));
	ASSERT_TRUE(object.contains(Neo::Vector2(5, 4)));
}
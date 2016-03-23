#include <gtest/gtest.h>
#include <NeoEngine.h>

using namespace Neo;

TEST(NeoEngineTest, FrustumVisibilityTest_true)
{
	OCamera camera;
	Frustum frustum;

	camera.getMatrix()->loadIdentity();
	int* viewport = camera.getCurrentViewport();

	viewport[2] = static_cast<int>(1024);
	viewport[3] = static_cast<int>(1024);

	Vector3 points[] = {
		Vector3(0,0, -10),
		Vector3(10,10,-30)
	};

	frustum.makeVolume(&camera);
	ASSERT_TRUE(frustum.isVolumePointsVisible(points, 2));
}

TEST(NeoEngineTest, FrustumVisibilityTest_false)
{
	OCamera camera;
	Frustum frustum;

	camera.getMatrix()->loadIdentity();
	int* viewport = camera.getCurrentViewport();

	viewport[2] = static_cast<int>(1024);
	viewport[3] = static_cast<int>(1024);

	Vector3 points[] = {
		Vector3(0,0, 5000),
		Vector3(10,1000,-30)
	};

	frustum.makeVolume(&camera);
	ASSERT_FALSE(frustum.isVolumePointsVisible(points, 2));
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
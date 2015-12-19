//========================================================================
// Copyright (c) 2015 Yannick Pflanzer <neo-engine.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <gtest/gtest.h>
#include <NeoCore.h>

using namespace std;
using namespace Neo;

static void printImage(Image& img)
{
	unsigned int pixel;
	for(int y = 0; y < img.getWidth(); y++)
	{
		for (int x = 0; x < img.getHeight(); x++)
		{
			img.readPixel(x, y, &pixel);
			printf("%3x ", pixel);
		}
		printf("\n");
	}
}

TEST(ImageTest, ReadPixel_test)
{
	Image image;
	unsigned int color = 0;

	image.create(VAR_INT, 5, 5, 1);

	*((unsigned int*) image.getData()) = 0xDEADBEEF;

	// Check pixel
	image.readPixel(0, 0, &color);
	ASSERT_EQ(0xDEADBEEF, color);
}

TEST(ImageTest, WritePixel_test)
{
	Image image;
	unsigned int color = 0xDEADBEEF;
	unsigned int written = 0;

	image.create(VAR_INT, 5, 5, 1);

	// Write pixel
	image.writePixel(3, 3, &color);

	// Check pixel
	image.readPixel(3, 3, &written);
	ASSERT_EQ(color, written);
}

TEST(ImageTest, Clear_test)
{
	Image image;
	unsigned int clearcolor = 0xDEADBEEF;

	image.create(VAR_INT, 5, 5, 1);
	image.clear(&clearcolor);

	// Check every pixel
	unsigned int pixel;
	for(int y = 0; y < image.getWidth(); y++)
		for(int x = 0; x < image.getHeight(); x++)
		{
			image.readPixel(x, y, &pixel);
			ASSERT_EQ(0xDEADBEEF, pixel);
		}
}

TEST(ImageTest, ScaleSmaller_test)
{
	Image image;
	image.create(VAR_UBYTE, 10, 10, 4);
	unsigned int pixel = 0;
	for(int y = 0; y < image.getWidth(); y++)
	{
		for (int x = 0; x < image.getHeight(); x++)
		{
			pixel = x + y;
			image.writePixel(x, y, &pixel);
		}
	}

	//printImage(image);
	//printf("\n\n");

	image.scale(0.5);
	//printImage(image);

	ASSERT_EQ(5, image.getWidth());
	ASSERT_EQ(5, image.getHeight());
}

TEST(ImageTest, ScaleBigger_test)
{
	Image image;
	image.create(VAR_UBYTE, 10, 10, 4);
	unsigned int pixel = 0;
	for(int y = 0; y < image.getWidth(); y++)
	{
		for (int x = 0; x < image.getHeight(); x++)
		{
			pixel = x + y;
			image.writePixel(x, y, &pixel);
		}
	}

	//printImage(image);
	//printf("\n\n");

	image.scale(2);
	//printImage(image);

	// TODO: Check image!
	ASSERT_EQ(20, image.getWidth());
	ASSERT_EQ(20, image.getHeight());
}

TEST(ImageTest, ScaleHuge_test)
{
	Image image;
	image.create(VAR_UBYTE, 1024, 1024, 4);
	unsigned int pixel = 0;
	for(int y = 0; y < image.getWidth(); y++)
	{
		for (int x = 0; x < image.getHeight(); x++)
		{
			pixel = x + y;
			image.writePixel(x, y, &pixel);
		}
	}

	//printImage(image);
	//printf("\n\n");

	image.scale(2);
	//printImage(image);

	// TODO: Check image!
	ASSERT_EQ(1024*2, image.getWidth());
	ASSERT_EQ(1024*2, image.getHeight());
}

TEST(ImageTest, Rotate360_test)
{
	Image image;
	image.create(VAR_UBYTE, 10, 10, 4);
	unsigned int pixel = 0;
	for(int y = 0; y < image.getWidth(); y++)
	{
		for (int x = 0; x < image.getHeight(); x++)
		{
			pixel = x + y;
			image.writePixel(x, y, &pixel);
		}
	}

	//printImage(image);
	//printf("\n\n");

	image.rotate(360);
	//printImage(image);
}

TEST(ImageTest, Rotate90_test)
{
	Image image;
	image.create(VAR_UBYTE, 4096, 4096, 4);
	unsigned int pixel = 0;
	for(int y = 0; y < image.getWidth(); y++)
	{
		for (int x = 0; x < image.getHeight(); x++)
		{
			pixel = x + y;
			image.writePixel(x, y, &pixel);
		}
	}

	//printImage(image);
	//printf("\n\n");

	image.rotate(90);
	//printImage(image);
}
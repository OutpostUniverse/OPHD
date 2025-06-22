#include <libOPHD/RandomNumberGenerator.h>

#include <gtest/gtest.h>


TEST(RandomNumberGenerator, IntType)
{
	EXPECT_EQ(0, randomNumber.generate(0, 0));
	EXPECT_EQ(1, randomNumber.generate(1, 1));
}


TEST(RandomNumberGenerator, FloatType)
{
	EXPECT_EQ(0.0f, randomNumber.generate(0.0f, 0.0f));
	EXPECT_EQ(1.0f, randomNumber.generate(1.0f, 1.0f));
}

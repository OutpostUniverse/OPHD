#include <libOPHD/RandomNumberGenerator.h>

#include <gtest/gtest.h>


TEST(RandomNumberGenerator, IntType)
{
	EXPECT_EQ(0, randomNumber.generate(0, 0));
	EXPECT_EQ(1, randomNumber.generate(1, 1));
}

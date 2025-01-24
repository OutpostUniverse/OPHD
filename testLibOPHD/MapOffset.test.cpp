#include <libOPHD/Map/MapOffset.h>

#include <gtest/gtest.h>


TEST(MapOffset, EqualOperator)
{
	EXPECT_TRUE((MapOffset{1, 2, 3}) == (MapOffset{1, 2, 3}));
	EXPECT_FALSE((MapOffset{1, 2, 3}) == (MapOffset{2, 1, 3}));
	EXPECT_FALSE((MapOffset{1, 2, 0}) == (MapOffset{1, 2, 3}));
}


TEST(MapOffset, NotEqualOperator)
{
	EXPECT_TRUE((MapOffset{1, 2, 3}) != (MapOffset{2, 1, 3}));
	EXPECT_TRUE((MapOffset{1, 2, 0}) != (MapOffset{1, 2, 3}));
	EXPECT_FALSE((MapOffset{1, 2, 3}) != (MapOffset{1, 2, 3}));
}


TEST(MapOffset, ScalarMultiplication)
{
	EXPECT_EQ((MapOffset{-1, 2, 3}) * 2, (MapOffset{-2, 4, 6}));
	EXPECT_EQ(2 * (MapOffset{-1, 2, 3}), (MapOffset{-2, 4, 6}));
	EXPECT_EQ((MapOffset{-1, 2, 3}) * -2, (MapOffset{2, -4, -6}));
	EXPECT_EQ((MapOffset{-1, 2, 3}) * 0, (MapOffset{0, 0, 0}));
}

TEST(MapOffset, ScalarMultiplicationWithAssignment)
{
	MapOffset offset = MapOffset{-1, 2, 3};
	EXPECT_EQ(offset *= 2, (MapOffset{-2, 4, 6}));
	EXPECT_EQ(offset *= -2, (MapOffset{4, -8, -12}));
	EXPECT_EQ(offset *= 0, (MapOffset{0, 0, 0}));
}

#include <libOPHD/DiggerUtility.h>

#include <gtest/gtest.h>


TEST(DiggerUtility, FacesAwayFromSingleExcavatedNeighbor)
{
	const std::array<bool, 8> neighborsNorthExcavated{true, false, false, false, false, false, false, false};
	EXPECT_EQ(Direction::South, chooseDiggerDirectionAwayFromExcavated(neighborsNorthExcavated));

	const std::array<bool, 8> neighborsWestExcavated{false, false, false, false, false, false, true, false};
	EXPECT_EQ(Direction::East, chooseDiggerDirectionAwayFromExcavated(neighborsWestExcavated));
}


TEST(DiggerUtility, PicksDiagonalForCornerBias)
{
	const std::array<bool, 8> neighborsNorthWestExcavated{false, false, false, false, false, false, false, true};
	EXPECT_EQ(Direction::SouthEast, chooseDiggerDirectionAwayFromExcavated(neighborsNorthWestExcavated));
}


TEST(DiggerUtility, FallbackToFirstUnexcavatedWhenBalanced)
{
	const std::array<bool, 8> balancedExcavation{false, false, true, false, false, false, true, false};
	EXPECT_EQ(Direction::North, chooseDiggerDirectionAwayFromExcavated(balancedExcavation));
}


TEST(DiggerUtility, AllNeighborsExcavatedDefaultsNorth)
{
	const std::array<bool, 8> allExcavated{true, true, true, true, true, true, true, true};
	EXPECT_EQ(Direction::North, chooseDiggerDirectionAwayFromExcavated(allExcavated));
}


TEST(DiggerUtility, DetectsAnyExcavatedNeighbor)
{
	const std::array<bool, 8> noneExcavated{};
	EXPECT_FALSE(hasExcavatedNeighbor(noneExcavated));

	const std::array<bool, 8> oneExcavated{false, false, false, true, false, false, false, false};
	EXPECT_TRUE(hasExcavatedNeighbor(oneExcavated));

	const std::array<bool, 8> multipleExcavated{true, false, true, false, false, false, false, false};
	EXPECT_TRUE(hasExcavatedNeighbor(multipleExcavated));
}


TEST(DiggerUtility, AutoPlacementRequiresNeighbor)
{
	const std::array<bool, 8> noneExcavated{};
	const auto decision = decideAutoDiggerPlacement(false, noneExcavated);
	EXPECT_FALSE(decision.canAutoPlace);
}


TEST(DiggerUtility, AutoPlacementPicksDirectionAndExcavates)
{
	const std::array<bool, 8> eastExcavated{false, false, true, false, false, false, false, false};
	const auto decision = decideAutoDiggerPlacement(false, eastExcavated);
	EXPECT_TRUE(decision.canAutoPlace);
	EXPECT_EQ(Direction::West, decision.direction);
}


TEST(DiggerUtility, AutoPlacementSkipsIfTileAlreadyExcavated)
{
	const std::array<bool, 8> westExcavated{false, false, false, false, false, false, true, false};
	const auto decision = decideAutoDiggerPlacement(true, westExcavated);
	EXPECT_FALSE(decision.canAutoPlace);
}

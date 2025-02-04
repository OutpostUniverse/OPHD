#include "DirectionOffset.h"

#include "EnumDirection.h"

#include <map>


namespace
{
	const std::map<Direction, NAS2D::Vector<int>> DirectionOffsetTable =
	{
		{ Direction::North, DirectionNorth },
		{ Direction::East, DirectionEast },
		{ Direction::West, DirectionWest },
		{ Direction::South, DirectionSouth },
		{ Direction::NorthEast, DirectionNorthEast },
		{ Direction::NorthWest, DirectionNorthWest },
		{ Direction::SouthEast, DirectionSouthEast },
		{ Direction::SouthWest, DirectionSouthWest },
		{ Direction::Up, DirectionCenter },
		{ Direction::Down, DirectionCenter }
	};
}


NAS2D::Vector<int> directionEnumToOffset(Direction direction)
{
	return DirectionOffsetTable.at(direction);
}


int directionEnumToVerticalOffset(Direction direction)
{
	switch(direction)
	{
		case Direction::Up:
			return -1;
		case Direction::Down:
			return 1;
		default:
			return 0;
	}
}

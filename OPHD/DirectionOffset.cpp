
#include "DirectionOffset.h"
#include "Common.h"


NAS2D::Vector<int> directionEnumToOffset(Direction direction)
{
	switch(direction)
	{
		case Direction::North:
			return DirectionNorth;
		case Direction::East:
			return DirectionEast;
		case Direction::South:
			return DirectionSouth;
		case Direction::West:
			return DirectionWest;
		default:
			return DirectionCenter;
	}
}

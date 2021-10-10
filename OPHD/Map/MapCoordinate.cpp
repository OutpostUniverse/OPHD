
#include "MapCoordinate.h"

#include "../DirectionOffset.h"


MapCoordinate MapCoordinate::offset(Direction direction) const
{
	return {
		xy + directionEnumToOffset(direction),
		z + directionEnumToVerticalOffset(direction)
	};
}

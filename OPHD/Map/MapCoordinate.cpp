
#include "MapCoordinate.h"

#include "../DirectionOffset.h"


MapCoordinate MapCoordinate::translate(Direction direction) const
{
	return {
		xy + directionEnumToOffset(direction),
		z + directionEnumToVerticalOffset(direction)
	};
}

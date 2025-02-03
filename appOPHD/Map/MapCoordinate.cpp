
#include "MapCoordinate.h"

#include <libOPHD/DirectionOffset.h>


MapCoordinate MapCoordinate::translate(MapOffset mapOffset) const
{
	return {
		xy + mapOffset.xy,
		z + mapOffset.z,
	};
}


MapCoordinate MapCoordinate::translate(Direction direction) const
{
	return {
		xy + directionEnumToOffset(direction),
		z + directionEnumToVerticalOffset(direction)
	};
}

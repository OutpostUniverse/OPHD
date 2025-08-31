
#include "MapCoordinate.h"

#include "MapOffset.h"
#include "../DirectionOffset.h"


MapCoordinate MapCoordinate::translate(NAS2D::Vector<int> offset) const
{
	return {
		xy + offset,
		z,
	};
}


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

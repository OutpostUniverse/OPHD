#pragma once

#include <NAS2D/Math/Point.h>


enum class Direction;


struct MapCoordinate
{
	NAS2D::Point<int> xy;
	int z;

	MapCoordinate translate(Direction direction) const;
};

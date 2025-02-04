#pragma once

#include "MapOffset.h"

#include <NAS2D/Math/Point.h>


enum class Direction;


struct MapCoordinate
{
	NAS2D::Point<int> xy;
	int z{0};

	MapCoordinate translate(MapOffset mapOffset) const;
	MapCoordinate translate(Direction direction) const;
};

#pragma once

#include <NAS2D/Math/Point.h>


enum class Direction;
struct MapOffset;


struct MapCoordinate
{
	NAS2D::Point<int> xy;
	int z{0};

	MapCoordinate translate(MapOffset mapOffset) const;
	MapCoordinate translate(Direction direction) const;
};

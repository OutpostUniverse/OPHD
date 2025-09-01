#pragma once

#include <NAS2D/Math/Point.h>


namespace NAS2D
{
	template <typename BaseType> struct Vector;
}


enum class Direction;
struct MapOffset;


struct MapCoordinate
{
	NAS2D::Point<int> xy;
	int z{0};

	MapCoordinate translate(NAS2D::Vector<int> offset) const;
	MapCoordinate translate(MapOffset mapOffset) const;
	MapCoordinate translate(Direction direction) const;
};

#pragma once

#include "Map/MapOffset.h"

#include <NAS2D/Math/Vector.h>

#include <array>


constexpr auto DirectionNorthWest = NAS2D::Vector{-1, -1};
constexpr auto DirectionNorth     = NAS2D::Vector{ 0, -1};
constexpr auto DirectionNorthEast = NAS2D::Vector{ 1, -1};
constexpr auto DirectionWest      = NAS2D::Vector{-1,  0};
constexpr auto DirectionCenter    = NAS2D::Vector{ 0,  0};
constexpr auto DirectionEast      = NAS2D::Vector{ 1,  0};
constexpr auto DirectionSouthWest = NAS2D::Vector{-1,  1};
constexpr auto DirectionSouth     = NAS2D::Vector{ 0,  1};
constexpr auto DirectionSouthEast = NAS2D::Vector{ 1,  1};


constexpr auto MapOffsetNorthWest = MapOffset{DirectionNorthWest, 0};
constexpr auto MapOffsetNorth     = MapOffset{DirectionNorth, 0};
constexpr auto MapOffsetNorthEast = MapOffset{DirectionNorthEast, 0};
constexpr auto MapOffsetWest      = MapOffset{DirectionWest, 0};
constexpr auto MapOffsetCenter    = MapOffset{DirectionCenter, 0};
constexpr auto MapOffsetEast      = MapOffset{DirectionEast, 0};
constexpr auto MapOffsetSouthWest = MapOffset{DirectionSouthWest, 0};
constexpr auto MapOffsetSouth     = MapOffset{DirectionSouth, 0};
constexpr auto MapOffsetSouthEast = MapOffset{DirectionSouthEast, 0};
constexpr auto MapOffsetUp        = MapOffset{DirectionCenter, -1};
constexpr auto MapOffsetDown      = MapOffset{DirectionCenter, 1};


constexpr auto DirectionScan3x3 = std::array{
	DirectionNorthWest,
	DirectionNorth,
	DirectionNorthEast,
	DirectionWest,
	DirectionCenter,
	DirectionEast,
	DirectionSouthWest,
	DirectionSouth,
	DirectionSouthEast
};

constexpr auto DirectionScan323 = std::array{
	DirectionNorthWest,
	DirectionNorth,
	DirectionNorthEast,
	DirectionWest,
	DirectionEast,
	DirectionSouthWest,
	DirectionSouth,
	DirectionSouthEast
};

constexpr auto DirectionClockwise8 = std::array{
	DirectionNorth,
	DirectionNorthEast,
	DirectionEast,
	DirectionSouthEast,
	DirectionSouth,
	DirectionSouthWest,
	DirectionWest,
	DirectionNorthWest
};

constexpr auto DirectionClockwise4 = std::array{
	DirectionNorth,
	DirectionEast,
	DirectionSouth,
	DirectionWest,
};


constexpr auto MapOffsetClockwise3x3x2 = std::array{
	MapOffsetNorthWest,
	MapOffsetNorth,
	MapOffsetNorthEast,
	MapOffsetWest,
	MapOffsetCenter,
	MapOffsetEast,
	MapOffsetSouthWest,
	MapOffsetSouth,
	MapOffsetSouthEast,
	MapOffsetUp,
	MapOffsetDown,
};


constexpr auto MapOffsetClockwise323x2 = std::array{
	MapOffsetNorthWest,
	MapOffsetNorth,
	MapOffsetNorthEast,
	MapOffsetWest,
	MapOffsetEast,
	MapOffsetSouthWest,
	MapOffsetSouth,
	MapOffsetSouthEast,
	MapOffsetUp,
	MapOffsetDown,
};


constexpr auto MapOffsetClockwise10 = std::array{
	MapOffsetNorth,
	MapOffsetNorthEast,
	MapOffsetEast,
	MapOffsetSouthWest,
	MapOffsetSouth,
	MapOffsetSouthEast,
	MapOffsetWest,
	MapOffsetNorthWest,
	MapOffsetUp,
	MapOffsetDown,
};


constexpr auto MapOffsetClockwise6 = std::array{
	MapOffsetNorth,
	MapOffsetEast,
	MapOffsetSouth,
	MapOffsetWest,
	MapOffsetUp,
	MapOffsetDown,
};


enum class Direction;

NAS2D::Vector<int> directionEnumToOffset(Direction direction);
int directionEnumToVerticalOffset(Direction direction);

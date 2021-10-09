#pragma once

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


enum class Direction;

NAS2D::Vector<int> directionEnumToOffset(Direction direction);

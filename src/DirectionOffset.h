#pragma once

#include "NAS2D/Renderer/Vector.h"

#include <array>


constexpr auto DirectionNorthWest = NAS2D::Vector<int>{-1, -1};
constexpr auto DirectionNorth     = NAS2D::Vector<int>{ 0, -1};
constexpr auto DirectionNorthEast = NAS2D::Vector<int>{ 1, -1};
constexpr auto DirectionWest      = NAS2D::Vector<int>{-1,  0};
constexpr auto DirectionCenter    = NAS2D::Vector<int>{ 0,  0};
constexpr auto DirectionEast      = NAS2D::Vector<int>{ 1,  0};
constexpr auto DirectionSouthWest = NAS2D::Vector<int>{-1,  1};
constexpr auto DirectionSouth     = NAS2D::Vector<int>{ 0,  1};
constexpr auto DirectionSouthEast = NAS2D::Vector<int>{ 1,  1};


constexpr auto DirectionScan3x3 = std::array<NAS2D::Vector<int>, 9>{
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

constexpr auto DirectionClockwise8 = std::array<NAS2D::Vector<int>, 8>{
	DirectionNorth,
	DirectionNorthEast,
	DirectionEast,
	DirectionSouthEast,
	DirectionSouth,
	DirectionSouthWest,
	DirectionWest,
	DirectionNorthWest
};

constexpr auto DirectionClockwise4 = std::array<NAS2D::Vector<int>, 4>{
	DirectionNorth,
	DirectionEast,
	DirectionSouth,
	DirectionWest,
};

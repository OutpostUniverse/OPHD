#include "DiggerUtility.h"

#include "DirectionOffset.h"

#include <NAS2D/Math/Vector.h>

#include <limits>


namespace
{
	constexpr std::array<Direction, 8> DirectionClockwiseEnum{
		Direction::North,
		Direction::NorthEast,
		Direction::East,
		Direction::SouthEast,
		Direction::South,
		Direction::SouthWest,
		Direction::West,
		Direction::NorthWest
	};


	int dotProduct(const NAS2D::Vector<int>& lhs, const NAS2D::Vector<int>& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}


	Direction directionForVector(const NAS2D::Vector<int>& desired)
	{
		auto bestDirection = DirectionClockwiseEnum.front();
		auto bestScore = std::numeric_limits<int>::min();

		for (std::size_t i = 0; i < DirectionClockwiseEnum.size(); ++i)
		{
			const auto score = dotProduct(desired, DirectionClockwise8[i]);
			if (score > bestScore)
			{
				bestScore = score;
				bestDirection = DirectionClockwiseEnum[i];
			}
		}

		return bestDirection;
	}
}


Direction chooseDiggerDirectionAwayFromExcavated(const std::array<bool, 8>& excavatedNeighbors)
{
	NAS2D::Vector<int> awayFromExcavated{0, 0};
	for (std::size_t i = 0; i < DirectionClockwiseEnum.size(); ++i)
	{
		if (excavatedNeighbors[i])
		{
			awayFromExcavated -= DirectionClockwise8[i];
		}
	}

	if (awayFromExcavated.x == 0 && awayFromExcavated.y == 0)
	{
		for (std::size_t i = 0; i < DirectionClockwiseEnum.size(); ++i)
		{
			if (excavatedNeighbors[i]) { continue; }
			return DirectionClockwiseEnum[i];
		}
	}

	return directionForVector(awayFromExcavated);
}


bool hasExcavatedNeighbor(const std::array<bool, 8>& neighbors)
{
	return std::any_of(neighbors.begin(), neighbors.end(), [](bool isExcavated) { return isExcavated; });
}


AutoDiggerPlacement decideAutoDiggerPlacement(bool tileExcavated, const std::array<bool, 8>& excavatedNeighbors)
{
	if (tileExcavated) { return {}; }
	if (!hasExcavatedNeighbor(excavatedNeighbors)) { return {}; }

	AutoDiggerPlacement result;
	result.canAutoPlace = true;
	result.direction = chooseDiggerDirectionAwayFromExcavated(excavatedNeighbors);
	return result;
}

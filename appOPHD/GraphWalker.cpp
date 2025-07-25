#include "GraphWalker.h"

#include "Map/Tile.h"
#include "Map/TileMap.h"
#include "MapObjects/Structure.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumDirection.h>
#include <libOPHD/DirectionOffset.h>

#include <libOPHD/Map/MapCoordinate.h>


using namespace NAS2D;


/**
 * Check which way a tube is facing to determine if it connects to the destination tube.
 * Broken off into its own function while fixing issue #11 to avoid code duplication.
 */
static bool checkSourceTubeAlignment(ConnectorDir srcConnectorDir, Direction direction)
{
	if (srcConnectorDir == ConnectorDir::Intersection || srcConnectorDir == ConnectorDir::Vertical)
	{
		return true;
	}
	else if (direction == Direction::East || direction == Direction::West)
	{
		if (srcConnectorDir == ConnectorDir::EastWest)
			return true;
	}
	else if (direction == Direction::North || direction == Direction::South)
	{
		if (srcConnectorDir == ConnectorDir::NorthSouth)
			return true;
	}

	return false;
}


/**
 * Utility function to check if there's a valid connection between src and dst.
 */
static bool validConnection(Structure& src, Structure& dst, Direction direction)
{
	const auto srcConnectorDir = src.connectorDirection();
	const auto dstConnectorDir = dst.connectorDirection();

	if (direction == Direction::Up || direction == Direction::Down)
	{
		return src.isConnector() && src.connectorDirection() == ConnectorDir::Vertical;
	}
	else if (dst.isConnector())
	{
		if (dstConnectorDir == ConnectorDir::Intersection || dstConnectorDir == ConnectorDir::Vertical)
		{
			return !src.isConnector() || checkSourceTubeAlignment(srcConnectorDir, direction);
		}
		else if (direction == Direction::East || direction == Direction::West)
		{
			return dstConnectorDir == ConnectorDir::EastWest;
		}
		else if (direction == Direction::North || direction == Direction::South)
		{
			return dstConnectorDir == ConnectorDir::NorthSouth;
		}
	}
	else
	{
		return src.isConnector() && checkSourceTubeAlignment(srcConnectorDir, direction);
	}

	return false;
}


void walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap)
{
	for (const auto& position : positions)
	{
		walkGraph(position, tileMap);
	}
}


void walkGraph(const MapCoordinate& position, TileMap& tileMap)
{
	Tile& thisTile = tileMap.getTile(position);
	thisTile.structure()->connected(true);

	const auto directions = std::array{
		Direction::Up,
		Direction::Down,
		Direction::North,
		Direction::East,
		Direction::South,
		Direction::West,
	};

	for (const auto direction : directions)
	{
		const auto nextPosition = position.translate(direction);
		if (!tileMap.isValidPosition(nextPosition)) { continue; }

		auto& tile = tileMap.getTile(nextPosition);
		if (!tile.hasStructure() || tile.structure()->connected()) { continue; }

		if (validConnection(*thisTile.structure(), *tile.structure(), direction))
		{
			walkGraph(nextPosition, tileMap);
		}
	}
}

#include "GraphWalker.h"

#include "Map/Tile.h"
#include "Map/TileMap.h"
#include "MapObjects/Structure.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumDirection.h>
#include <libOPHD/DirectionOffset.h>

#include <libOPHD/Map/MapCoordinate.h>


namespace
{
	bool canConnect(Structure& src, Structure& dst, Direction direction)
	{
		const auto srcConnectorDir = src.connectorDirection();
		const auto dstConnectorDir = dst.connectorDirection();

		// At least one end must be a Tube as Structures don't connect to each other
		if (!src.isConnector() && !dst.isConnector()) { return false; }

		// Only follow directions that are valid for source connector
		if (!hasConnectorDirection(srcConnectorDir, direction)) { return false; }

		// Check if destination can receive a connection from the given direction
		// (Relies on symmetry of connector directions)
		return hasConnectorDirection(dstConnectorDir, direction);
	}
}


bool hasConnectorDirection(ConnectorDir srcConnectorDir, Direction direction)
{
	if (srcConnectorDir == ConnectorDir::Vertical)
	{
		return true;
	}
	else if (srcConnectorDir == ConnectorDir::Intersection)
	{
		return direction == Direction::North || direction == Direction::South || direction == Direction::East || direction == Direction::West;
	}
	else if (srcConnectorDir == ConnectorDir::NorthSouth)
	{
		return direction == Direction::North || direction == Direction::South;
	}
	else if (srcConnectorDir == ConnectorDir::EastWest)
	{
		return direction == Direction::East || direction == Direction::West;
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

		auto& nextTile = tileMap.getTile(nextPosition);
		if (!nextTile.hasStructure() || nextTile.structure()->connected()) { continue; }

		if (canConnect(*thisTile.structure(), *nextTile.structure(), direction))
		{
			walkGraph(nextPosition, tileMap);
		}
	}
}

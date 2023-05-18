#include "GraphWalker.h"
#include "DirectionOffset.h"

#include "Map/MapCoordinate.h"
#include "Map/TileMap.h"
#include "MapObjects/Structure.h"


using namespace NAS2D;


/**
 * Check which way a tube is facing to determine if it connects to the destination tube.
 * Broken off into its own function while fixing issue #11 to avoid code duplication.
 */
static bool checkSourceTubeAlignment(Structure* src, Direction direction)
{
	if (src->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || src->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
	{
		return true;
	}
	else if (direction == Direction::East || direction == Direction::West)
	{
		if (src->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT)
			return true;
	}
	else if (direction == Direction::North || direction == Direction::South)
	{
		if (src->connectorDirection() == ConnectorDir::CONNECTOR_LEFT)
			return true;
	}

	return false;
}


/**
 * Utility function to check if there's a valid connection between src and dst.
 */
static bool validConnection(Structure* src, Structure* dst, Direction direction)
{
	if (src == nullptr || dst == nullptr)
	{
		throw std::runtime_error("GraphWalker::validConnection() was passed a NULL Pointer.");
	}
	if (direction == Direction::Up || direction == Direction::Down)
	{
		if (src->isConnector() && src->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL) { return true; }
		return false;
	}
	else if (dst->isConnector())
	{
		if (dst->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || dst->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
		{
			if (!src->isConnector())
			{
				return true;
			}
			else
			{
				return checkSourceTubeAlignment(src, direction);
			}
		}
		else if (direction == Direction::East || direction == Direction::West)
		{
			if (dst->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT) { return true; }
		}
		else if (direction == Direction::North || direction == Direction::South)
		{
			if (dst->connectorDirection() == ConnectorDir::CONNECTOR_LEFT) { return true; }
		}

		return false;
	}
	else if (src->isConnector())
	{
		return checkSourceTubeAlignment(src, direction);
	}

	return false;
}


std::vector<Tile*> walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap)
{
	std::vector<Tile*> tileList;
	for (const auto& position : positions)
	{
		walkGraph(position, tileMap, tileList);
	}
	return tileList;
}


void walkGraph(const MapCoordinate& position, TileMap& tileMap, std::vector<Tile*>& tileList)
{
	Tile& thisTile = tileMap.getTile(position);
	thisTile.connected(true);
	tileList.push_back(&thisTile);

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
		if (!tile.thingIsStructure() || tile.connected() || tile.mine() || !tile.excavated()) { continue; }

		if (validConnection(thisTile.structure(), tile.structure(), direction))
		{
			walkGraph(nextPosition, tileMap, tileList);
		}
	}
}

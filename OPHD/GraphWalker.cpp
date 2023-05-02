#include "GraphWalker.h"
#include "DirectionOffset.h"

#include "Map/TileMap.h"
#include "Things/Structures/Structure.h"


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


GraphWalker::GraphWalker(const MapCoordinate& position, TileMap& tileMap, std::vector<Tile*>& tileList) :
	mTileMap{tileMap},
	mThisTile{tileMap.getTile(position)},
	mTileList{tileList},
	mPosition{position}
{
	walkGraph();
}


void GraphWalker::walkGraph()
{
	mThisTile.connected(true);
	mTileList.push_back(&mThisTile);

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
		check(mPosition, direction);
	}
}


/**
 * Checks a given map location for a valid connection.
 *
 * \todo	With Tile being updated to include position information, this function can be modified
 *			to take a source and destination tile instead of looking them up. By using the internal
 *			positional information in the Tiles we can deduce direction between source and destination.
 */
void GraphWalker::check(const MapCoordinate& fromPosition, Direction direction)
{
	const auto position = fromPosition.translate(direction);
	if (!mTileMap.isValidPosition(position)) { return; }

	auto& tile = mTileMap.getTile(position);
	if (tile.connected() || tile.mine() || !tile.excavated() || !tile.thingIsStructure()) { return; }

	if (validConnection(mThisTile.structure(), tile.structure(), direction))
	{
		GraphWalker walker(position, mTileMap, mTileList);
	}
}

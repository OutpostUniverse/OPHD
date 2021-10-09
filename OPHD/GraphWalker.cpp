#include "GraphWalker.h"
#include "DirectionOffset.h"

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


GraphWalker::GraphWalker(const NAS2D::Point<int>& point, int depth, TileMap& tileMap, TileList& tileList) :
	mTileMap{tileMap},
	mThisTile{tileMap.getTile({point, depth})},
	mTileList{tileList},
	mGridPosition{point},
	mDepth{depth}
{
	walkGraph();
}


void GraphWalker::walkGraph()
{
	mThisTile.connected(true);
	mTileList.push_back(&mThisTile);

	if (mDepth > 0) { check(mGridPosition, mDepth - 1, Direction::Up); }
	if (mDepth < mTileMap.maxDepth()) { check(mGridPosition, mDepth + 1, Direction::Down); }

	check(mGridPosition + DirectionNorth, mDepth, Direction::North);
	check(mGridPosition + DirectionEast, mDepth, Direction::East);
	check(mGridPosition + DirectionSouth, mDepth, Direction::South);
	check(mGridPosition + DirectionWest, mDepth, Direction::West);
}


/**
 * Checks a given map location for a valid connection.
 *
 * \todo	With Tile being updated to include position information, this function can be modified
 *			to take a source and destination tile instead of looking them up. By using the internal
 *			positional information in the Tiles we can deduce direction between source and destination.
 */
void GraphWalker::check(NAS2D::Point<int> point, int depth, Direction direction)
{
	if (!NAS2D::Rectangle<int>::Create({0, 0}, mTileMap.size()).contains(point)) { return; }
	if (depth < 0 || depth > mTileMap.maxDepth()) { return; }

	auto& tile = mTileMap.getTile({point, depth});

	if (tile.connected() || tile.mine() || !tile.excavated() || !tile.thingIsStructure()) { return; }

	if (validConnection(mThisTile.structure(), tile.structure(), direction))
	{
		GraphWalker walker(point, depth, mTileMap, mTileList);
	}
}

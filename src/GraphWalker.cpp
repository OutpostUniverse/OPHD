// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GraphWalker.h"

using namespace NAS2D;


/**
 * Check which way a tube is facing to determine if it connects to the destination tube.
 * Broken off into its own function while fixing issue #11 to avoid code duplication.
 */
static bool checkSourceTubeAlignment(Structure* src, Direction _d)
{
	if (src->connectorDirection() == CONNECTOR_INTERSECTION || src->connectorDirection() == CONNECTOR_VERTICAL)
	{
		return true;
	}
	else if (_d == DIR_EAST || _d == DIR_WEST)
	{
		if (src->connectorDirection() == CONNECTOR_RIGHT)
			return true;
	}
	else if (_d == DIR_NORTH || _d == DIR_SOUTH)
	{
		if (src->connectorDirection() == CONNECTOR_LEFT)
			return true;
	}

	return false;
}


/**
 * Utility function to check if there's a valid connection between src and dst.
 */
static bool validConnection(Structure* src, Structure* dst, Direction _d)
{
	if (src == nullptr || dst == nullptr)
	{
		throw std::runtime_error("GraphWalker::validConnection() was passed a NULL Pointer.");
	}
	if (_d == DIR_UP || _d == DIR_DOWN)
	{
		if (src->isConnector() && src->connectorDirection() == CONNECTOR_VERTICAL) { return true; }
		return false;
	}
	else if (dst->isConnector())
	{
		if (dst->connectorDirection() == CONNECTOR_INTERSECTION || dst->connectorDirection() == CONNECTOR_VERTICAL)
		{
			if (!src->isConnector()) { return true; }
			else { return checkSourceTubeAlignment(src, _d); }
		}
		else if (_d == DIR_EAST || _d == DIR_WEST)
		{
			if (dst->connectorDirection() == CONNECTOR_RIGHT) { return true; }
		}
		else if (_d == DIR_NORTH || _d == DIR_SOUTH)
		{
			if (dst->connectorDirection() == CONNECTOR_LEFT) { return true; }
		}

		return false;
	}
	else if (src->isConnector())
	{
		return checkSourceTubeAlignment(src, _d);
	}

	return false;
}


GraphWalker::GraphWalker(const NAS2D::Point<int>& point, int depth, TileMap* tileMap) :
	mTileMap(tileMap),
	mThisTile(tileMap->getTile(point, depth)),
	mGridPosition(point),
	mDepth(depth)
{
	walkGraph();
}


void GraphWalker::walkGraph()
{
	mThisTile->connected(true);

	if (mDepth > 0) { check(mGridPosition.x(), mGridPosition.y(), mDepth - 1, DIR_UP); }
	if (mDepth < mTileMap->maxDepth()) { check(mGridPosition.x(), mGridPosition.y(), mDepth + 1, DIR_DOWN); }

	check(mGridPosition.x(), mGridPosition.y() - 1, mDepth, DIR_NORTH);
	check(mGridPosition.x() + 1, mGridPosition.y(), mDepth, DIR_EAST);
	check(mGridPosition.x(), mGridPosition.y() + 1, mDepth, DIR_SOUTH);
	check(mGridPosition.x() - 1, mGridPosition.y(), mDepth, DIR_WEST);
}


/**
 * Checks a given map location for a valid connection.
 *
 * \todo	With Tile being updated to include position information, this function can be modified
 *			to take a source and destination tile instead of looking them up. By using the internal
 *			positional information in the Tiles we can deduce direction between source and destination.
 */
void GraphWalker::check(int x, int y, int depth, Direction direction)
{
	const NAS2D::Point point{x, y};
	if (!NAS2D::Rectangle<int>::Create({0, 0}, mTileMap->size()).contains(point)) { return; }
	if (depth < 0 || depth > mTileMap->maxDepth()) { return; }

	Tile* tile = mTileMap->getTile(point, depth);

	if (tile->connected() || tile->mine() || !tile->excavated() || !tile->thingIsStructure()) { return; }

	if (validConnection(mThisTile->structure(), tile->structure(), direction))
	{
		GraphWalker walker(point, depth, mTileMap);
	}
}

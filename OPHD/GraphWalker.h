#pragma once

#include "Common.h"

#include "Map/TileMap.h"

#include <NAS2D/Math/Point.h>


/**
 * GraphWalker does a basic depth-first connection check
 *			on a TileMap given a starting point.
 */
class GraphWalker
{
public:
	GraphWalker(const MapCoordinate&, TileMap&, TileList&);
	~GraphWalker() = default;

private:
	GraphWalker() = delete;
	GraphWalker(GraphWalker&) = delete;
	GraphWalker& operator=(const GraphWalker&) = delete;

private:
	void walkGraph();
	void check(const MapCoordinate& fromPosition, Direction direction);

private:
	TileMap& mTileMap;
	Tile& mThisTile;
	TileList& mTileList;

	NAS2D::Point<int> mGridPosition;
	int mDepth{0};
};

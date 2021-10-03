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
	GraphWalker(const NAS2D::Point<int>&, int, TileMap&, TileList&);
	~GraphWalker() = default;

private:
	GraphWalker() = delete;
	GraphWalker(GraphWalker&) = delete;
	GraphWalker& operator=(const GraphWalker&) = delete;

private:
	void walkGraph();
	void check(NAS2D::Point<int> point, int depth, Direction direction);

private:
	TileMap& mTileMap;
	Tile& mThisTile;
	TileList& mTileList;

	NAS2D::Point<int> mGridPosition;
	int mDepth{0};
};

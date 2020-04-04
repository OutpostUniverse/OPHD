#pragma once

#include "Map/TileMap.h"

#include "NAS2D/Renderer/Point.h"


/**
 * \brief	GraphWalker does a basic depth-first connection check
 *			on a TileMap given a starting point.
 */
class GraphWalker
{
public:
	GraphWalker(const NAS2D::Point<int>& point, int depth, TileMap* tileMap);
	~GraphWalker() = default;
	
private:
	GraphWalker() = delete;
	GraphWalker(GraphWalker&) = delete;
	GraphWalker& operator=(const GraphWalker&) = delete;

private:
	void walkGraph();
	void check(NAS2D::Point<int> point, int depth, Direction direction);

private:
	TileMap* mTileMap = nullptr;
	Tile* mThisTile = nullptr;

	NAS2D::Point<int> mGridPosition;
	int mDepth = 0;
};


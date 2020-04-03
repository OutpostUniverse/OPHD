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
	GraphWalker(const NAS2D::Point_2d&, int, TileMap*);
	~GraphWalker() = default;
	
private:
	GraphWalker() = delete;
	GraphWalker(GraphWalker&) = delete;
	GraphWalker& operator=(const GraphWalker&) = delete;

private:
	void walkGraph();
	void check(int x, int y, int depth, Direction direction);

private:
	TileMap*		mTileMap = nullptr;

	Tile*			mThisTile = nullptr;

	NAS2D::Point_2d	mGridPosition;
	int				mDepth = 0;
};


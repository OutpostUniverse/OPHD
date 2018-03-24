#pragma once

#include "Map/TileMap.h"


/**
 *
 */
class GraphWalker
{
public:
	GraphWalker(const Point_2d& _p, int _d, TileMap* _t);
	~GraphWalker();
	
private:
	GraphWalker() = delete;
	GraphWalker(GraphWalker&) = delete;
	GraphWalker& operator=(const GraphWalker&) = delete;

private:
	void walkGraph();
	void check(int x, int y, int depth, Direction _d);

private:
	TileMap*	_tileMap;

	Tile*		_thisTile;

	Point_2d	_gridPosition;
	int			_depth;
};


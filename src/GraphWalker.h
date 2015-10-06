#ifndef __GRAPHWALKER__
#define __GRAPHWALKER__

#include "TileMap.h"


/**
 *  
 */
class GraphWalker
{
public:

	GraphWalker(Point_2d& _p, int _d, TileMap* _t);
	~GraphWalker();

	

private:
	
	GraphWalker() {}	// No default c'tor
	GraphWalker(GraphWalker&) {}	// No copy c'tor

	void walkGraph();
	void check(int x, int y, int depth, Direction _d);
	
	TileMap*	_tileMap;

	Tile*		_thisTile;

	Point_2d	_gridPosition;
	int			_depth;
};

#endif

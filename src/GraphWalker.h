#ifndef __GRAPHWALKER__
#define __GRAPHWALKER__

#include "TileMap.h"


/**
 *  
 */
class GraphWalker
{
public:

	GraphWalker();
	~GraphWalker();

	void gridPosition(Point_2d& _p);
	void depth(int _d) { _depth = _d; }

	/** SET THIS LAST! */
	void tileMap(TileMap* _t);

	void walkGraph();

private:
	void check(int x, int y, Direction _d);
	
	TileMap*	_tileMap;

	Tile*		_thisTile;

	Point_2d	_gridPosition;
	int			_depth;
};

#endif

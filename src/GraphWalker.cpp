#include "GraphWalker.h"


/**
 * Utility function to check if there's a valid connection between
 */
bool validConnection(Structure* src, Structure* dst, Direction _d)
{
	if (_d == DIR_UP || _d == DIR_DOWN)
	{
		if (src->isConnector() && src->connectorDirection() == CONNECTOR_VERTICAL)
			return true;
		
		return false;
	}
	else if (dst->isConnector())
	{
		if (dst->connectorDirection() == CONNECTOR_INTERSECTION || dst->connectorDirection() == CONNECTOR_VERTICAL)
		{
			return true;
		}
		else if (_d == DIR_EAST || _d == DIR_WEST)
		{
			if (dst->connectorDirection() == CONNECTOR_RIGHT)
				return true;
		}
		else if (_d == DIR_NORTH || _d == DIR_SOUTH)
		{
			if (dst->connectorDirection() == CONNECTOR_LEFT)
				return true;
		}
		
		return false;
	}
	else if(src->isConnector())
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

	return false;
}


GraphWalker::GraphWalker(Point_2d& _p, int _d, TileMap* _t) :	_tileMap(_t),
																_thisTile(_t->getTile(_p.x(), _p.y(), _d)),
																_gridPosition(_p),
																_depth(_d)
{
	walkGraph();
}


GraphWalker::~GraphWalker()
{
	_tileMap = nullptr;
}

void GraphWalker::walkGraph()
{
	_thisTile->connected(true);


	if (_depth > 0)
		check(_gridPosition.x(), _gridPosition.y(), _depth - 1, DIR_UP);
	if(_depth < _tileMap->maxDepth())
		check(_gridPosition.x(), _gridPosition.y(), _depth + 1, DIR_DOWN);

	check(_gridPosition.x(), _gridPosition.y() - 1, _depth, DIR_NORTH);
	check(_gridPosition.x() + 1, _gridPosition.y(), _depth, DIR_EAST);
	check(_gridPosition.x(), _gridPosition.y() + 1, _depth, DIR_SOUTH);
	check(_gridPosition.x() - 1, _gridPosition.y(), _depth, DIR_WEST);
}


void GraphWalker::check(int x, int y, int depth, Direction _d)
{
	if (x < 0 || x > _tileMap->width() - 1 || y < 0 || y > _tileMap->height() - 1)
		return;
	if (depth < 0 || depth > _tileMap->maxDepth())
		return;


	Tile* t = _tileMap->getTile(x, y, depth);

	if (t->connected() || t->mine() || !t->excavated() || !t->thingIsStructure())
		return;

	// FIXME: Really hate these casts... got to be a better way to do this.
	Structure* src = _thisTile->structure();
	Structure* dst = t->structure();

	if (validConnection(src, dst, _d))
	{
		GraphWalker walker(Point_2d(x, y), depth, _tileMap);
	}
}

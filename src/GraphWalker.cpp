#include "GraphWalker.h"


/**
 * Utility function to check if there's a valid connection between
 */
bool validConnection(Structure* src, Structure* dst, Direction _d)
{

	if (dst->isConnector())
	{
		if (dst->connectorDirection() == CONNECTOR_INTERSECTION)
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
		else
		{
			return false;
		}
	}
	else if(src->isConnector())
	{
		if (src->connectorDirection() == CONNECTOR_INTERSECTION)
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
		else
		{
			return false;
		}
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
	_tileMap->getTile(_gridPosition.x(), _gridPosition.y(), _depth)->connected(true);

	check(_gridPosition.x(), _gridPosition.y() - 1, DIR_NORTH);
	check(_gridPosition.x() + 1, _gridPosition.y(), DIR_EAST);
	check(_gridPosition.x(), _gridPosition.y() + 1, DIR_SOUTH);
	check(_gridPosition.x() - 1, _gridPosition.y(), DIR_WEST);
}


void GraphWalker::check(int x, int y, Direction _d)
{
	if (x < 0 || x > _tileMap->width() - 1 || y < 0 || y > _tileMap->height() - 1)
		return;

	Tile* t = _tileMap->getTile(x, y);

	if (t->connected() || t->mine() || !t->excavated() || !t->thingIsStructure())
		return;

	Structure* src = reinterpret_cast<Structure*>(_thisTile->thing());
	Structure* dst = reinterpret_cast<Structure*>(t->thing());

	if (validConnection(src, dst, _d))
	{
		GraphWalker walker(Point_2d(x, y), _depth, _tileMap);
	}
}

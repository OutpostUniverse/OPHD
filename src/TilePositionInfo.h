#ifndef __TILE_POSITION_INFO__
#define __TILE_POSITION_INFO__

#include "Tile.h"

struct TilePositionInfo
{
	TilePositionInfo(): tile(NULL), x(0), y(0), depth(0)
	{}

	TilePositionInfo(Tile* _tile, int x, int y, int depth): tile(_tile), x(x), y(y), depth(depth)
	{}

	TilePositionInfo(TilePositionInfo const& copy): tile(copy.tile), x(copy.x), y(copy.y), depth(copy.depth)
	{}

	~TilePositionInfo()
	{}

	TilePositionInfo& operator=(TilePositionInfo const& copy)
	{
		tile = copy.tile;
		x = copy.x;
		y = copy.y;
		depth = copy.depth;

		return *this;
	}

	void operator()(Tile* _t, int _x, int _y, int _d)
	{
		tile = _t;
		x = _x;
		y = _y;
		depth = _d;
	}

	Tile*		tile;
	int			depth;
	int			x;
	int			y;
};


#endif
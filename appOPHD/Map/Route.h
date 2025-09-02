#pragma once

#include <vector>


class Tile;


struct Route
{
	std::vector<Tile*> path;
	int cost = 0;

	bool isEmpty() const { return path.empty(); }
};

#pragma once

#include <vector>


class Tile;


struct Route
{
	std::vector<Tile*> path;
	float cost = 0.0f;

	bool isEmpty() const { return path.empty(); }
};

#pragma once

#include <map>
#include <vector>

struct Route
{
	std::vector<void*> path;
	float cost = 0.0f;

	bool empty() const { return path.empty(); }
};

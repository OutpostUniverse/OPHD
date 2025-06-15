#pragma once

#include <map>
#include <vector>

struct Route
{
	bool empty() const { return path.empty(); }

	std::vector<void*> path;
	float cost = 0.0f;
};

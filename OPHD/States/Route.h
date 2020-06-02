#pragma once

#include <map>
#include <vector>

struct Route
{
	bool empty() const { return mPath.empty(); }

	std::vector<void*>	mPath;
	float mCost = 0.0f;
};

using RouteList = std::vector<Route>;

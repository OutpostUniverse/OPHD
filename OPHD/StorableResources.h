#pragma once

#include <array>

struct StorableResources
{
	StorableResources() = default;

	StorableResources operator+(const StorableResources& other) const
	{
		StorableResources out;

		for (size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = resources[i] + other.resources[i];
		}

		return out;
	}

	StorableResources operator-(const StorableResources& other) const
	{
		StorableResources out;

		for (size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = resources[i] - other.resources[i];
		}

		return out;
	}

	bool operator<=(const StorableResources& other) const
	{
		return resources[0] <= other.resources[0] &&
			resources[1] <= other.resources[1] &&
			resources[2] <= other.resources[2] &&
			resources[3] <= other.resources[3];
	}

	bool operator>=(const StorableResources& other) const
	{
		return other.resources <= resources;
	}

	StorableResources cap(int max) const
	{
		StorableResources out;
		for (std::size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = std::clamp(resources[i], 0, max);
		}

		return out;
	}

	void clear()
	{
		resources = { 0, 0, 0, 0 };
	}

	std::array<int, 4> resources{ 0 };
};

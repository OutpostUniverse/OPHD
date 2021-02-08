#pragma once

#include <algorithm>
#include <array>


struct StorableResources
{
	StorableResources& operator+=(const StorableResources& other)
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			resources[i] += other.resources[i];
		}

		return *this;
	}

	StorableResources& operator-=(const StorableResources& other)
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			resources[i] -= other.resources[i];
		}

		return *this;
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

	bool operator<(const StorableResources& other) const
	{
		return resources[0] < other.resources[0] &&
			resources[1] < other.resources[1] &&
			resources[2] < other.resources[2] &&
			resources[3] < other.resources[3];
	}

	bool operator>(const StorableResources& other) const
	{
		return other.resources < resources;
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

	bool isEmpty() const
	{
		if (resources[0] > 0 ||
			resources[1] > 0 ||
			resources[2] > 0 ||
			resources[3] > 0)
		{
			return false;
		}

		return true;
	}

	std::array<int, 4> resources{};
};


inline StorableResources operator+(StorableResources lhs, const StorableResources& rhs)
{
	return lhs += rhs;
}

inline StorableResources operator-(StorableResources lhs, const StorableResources& rhs)
{
	return lhs -= rhs;
}

#pragma once

#include <algorithm>
#include <array>


struct StorableResources
{
	constexpr StorableResources& operator+=(const StorableResources& other)
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			resources[i] += other.resources[i];
		}

		return *this;
	}

	constexpr StorableResources& operator-=(const StorableResources& other)
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			resources[i] -= other.resources[i];
		}

		return *this;
	}

	constexpr bool operator<=(const StorableResources& other) const
	{
		return resources[0] <= other.resources[0] &&
			resources[1] <= other.resources[1] &&
			resources[2] <= other.resources[2] &&
			resources[3] <= other.resources[3];
	}

	constexpr bool operator>=(const StorableResources& other) const
	{
		return other <= *this;
	}

	constexpr bool operator<(const StorableResources& other) const
	{
		return resources[0] < other.resources[0] &&
			resources[1] < other.resources[1] &&
			resources[2] < other.resources[2] &&
			resources[3] < other.resources[3];
	}

	constexpr bool operator>(const StorableResources& other) const
	{
		return other < *this;
	}

	constexpr StorableResources cap(int max) const
	{
		StorableResources out;
		for (std::size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = std::clamp(resources[i], 0, max);
		}

		return out;
	}

	constexpr bool isEmpty() const
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


constexpr StorableResources operator+(StorableResources lhs, const StorableResources& rhs)
{
	return lhs += rhs;
}

constexpr StorableResources operator-(StorableResources lhs, const StorableResources& rhs)
{
	return lhs -= rhs;
}

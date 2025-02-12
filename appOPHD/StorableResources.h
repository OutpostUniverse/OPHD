#pragma once

#include <algorithm>
#include <array>
#include <numeric>


struct StorableResources
{
	constexpr StorableResources operator*(int multiplier) const
	{
		StorableResources result = *this;
		for (size_t i = 0; i < resources.size(); ++i)
		{
			result.resources[i] *= multiplier;
		}
		return result;
	}

	constexpr StorableResources operator/(int divisor) const
	{
		StorableResources result = *this;
		for (size_t i = 0; i < resources.size(); ++i)
		{
			result.resources[i] /= divisor;
		}
		return result;
	}

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
		for (size_t i = 0; i < resources.size(); ++i)
		{
			if (!(resources[i] <= other.resources[i]))
			{
				return false;
			}
		}
		return true;
	}

	constexpr bool operator<(const StorableResources& other) const
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			if (!(resources[i] < other.resources[i]))
			{
				return false;
			}
		}
		return true;
	}

	constexpr bool operator>=(const StorableResources& other) const
	{
		return other <= *this;
	}

	constexpr bool operator>(const StorableResources& other) const
	{
		return other < *this;
	}

	constexpr StorableResources cap(const StorableResources& other) const
	{
		StorableResources out;
		for (std::size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = std::clamp(resources[i], 0, other.resources[i]);
		}

		return out;
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
		for (size_t i = 0; i < resources.size(); ++i)
		{
			if (resources[i] > 0)
			{
				return false;
			}
		}
		return true;
	}


	// Sum of all resource types
	int total() const
	{
		return std::accumulate(resources.begin(), resources.end(), 0);
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

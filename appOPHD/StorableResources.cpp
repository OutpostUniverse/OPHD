#include "StorableResources.h"

#include <algorithm>
#include <numeric>


StorableResources StorableResources::operator*(int multiplier) const
{
	StorableResources result = *this;
	for (size_t i = 0; i < resources.size(); ++i)
	{
		result.resources[i] *= multiplier;
	}
	return result;
}


StorableResources StorableResources::operator/(int divisor) const
{
	StorableResources result = *this;
	for (size_t i = 0; i < resources.size(); ++i)
	{
		result.resources[i] /= divisor;
	}
	return result;
}


StorableResources& StorableResources::operator+=(const StorableResources& other)
{
	for (size_t i = 0; i < resources.size(); ++i)
	{
		resources[i] += other.resources[i];
	}

	return *this;
}


StorableResources& StorableResources::operator-=(const StorableResources& other)
{
	for (size_t i = 0; i < resources.size(); ++i)
	{
		resources[i] -= other.resources[i];
	}

	return *this;
}


bool StorableResources::operator<=(const StorableResources& other) const
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


bool StorableResources::operator<(const StorableResources& other) const
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


bool StorableResources::operator>=(const StorableResources& other) const
{
	return other <= *this;
}


bool StorableResources::operator>(const StorableResources& other) const
{
	return other < *this;
}


StorableResources StorableResources::cap(const StorableResources& other) const
{
	StorableResources out;
	for (std::size_t i = 0; i < resources.size(); ++i)
	{
		out.resources[i] = std::clamp(resources[i], 0, other.resources[i]);
	}

	return out;
}


StorableResources StorableResources::cap(int max) const
{
	StorableResources out;
	for (std::size_t i = 0; i < resources.size(); ++i)
	{
		out.resources[i] = std::clamp(resources[i], 0, max);
	}

	return out;
}


bool StorableResources::isEmpty() const
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
int StorableResources::total() const
{
	return std::accumulate(resources.begin(), resources.end(), 0);
}


StorableResources operator+(StorableResources lhs, const StorableResources& rhs)
{
	return lhs += rhs;
}


StorableResources operator-(StorableResources lhs, const StorableResources& rhs)
{
	return lhs -= rhs;
}

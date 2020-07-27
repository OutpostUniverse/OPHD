#pragma once

#include <array>

struct StoredResources
{
	StoredResources() = default;

	StoredResources operator+(const StoredResources& other) const
	{
		StoredResources out;

		for (size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = resources[i] + other.resources[i];
		}

		return out;
	}

	StoredResources operator-(const StoredResources& other) const
	{
		StoredResources out;

		for (size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = resources[i] - other.resources[i];
		}

		return out;
	}

	bool operator<=(const StoredResources& other) const
	{
		for (std::size_t i = 0; i < resources.size(); ++i)
		{
			if (resources[i] > other.resources[i])
			{
				return false;
			}
		}
		return true;
	}

	StoredResources cap(uint32_t max) const
	{
		StoredResources out;
		for (std::size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = std::clamp(resources[i], 0u, max);
		}

		return out;
	}

	std::array<uint32_t, 4> resources{ 0 };
};

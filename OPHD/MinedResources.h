#pragma once

#include <array>

struct MinedResources
{
	MinedResources() = default;

	MinedResources operator+(const MinedResources& other) const
	{
		MinedResources out;

		for (size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = resources[i] + other.resources[i];
		}

		return out;
	}

	MinedResources operator-(const MinedResources& other) const
	{
		MinedResources out;

		for (size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = resources[i] - other.resources[i];
		}

		return out;
	}

	bool operator<=(const MinedResources& other) const
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

	MinedResources cap(uint32_t max) const
	{
		MinedResources out;
		for (std::size_t i = 0; i < resources.size(); ++i)
		{
			out.resources[i] = std::clamp(resources[i], 0u, max);
		}

		return out;
	}

	std::array<uint32_t, 4> resources{ 0 };
};

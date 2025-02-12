#pragma once

#include <array>


struct StorableResources
{
	StorableResources operator*(int multiplier) const;
	StorableResources operator/(int divisor) const;

	StorableResources& operator+=(const StorableResources& other);
	StorableResources& operator-=(const StorableResources& other);

	bool operator<=(const StorableResources& other) const;
	bool operator<(const StorableResources& other) const;
	bool operator>=(const StorableResources& other) const;
	bool operator>(const StorableResources& other) const;

	StorableResources cap(const StorableResources& other) const;
	StorableResources cap(int max) const;

	bool isEmpty() const;
	int total() const;

	std::array<int, 4> resources{};
};


StorableResources operator+(StorableResources lhs, const StorableResources& rhs);
StorableResources operator-(StorableResources lhs, const StorableResources& rhs);

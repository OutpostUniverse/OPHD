#pragma once

#include <array>

struct PopulationTable
{
public:
	enum class Role
	{
		Child,
		Student,
		Worker,
		Scientist,
		Retired
	};

	PopulationTable() = default;
	PopulationTable(std::array<int, 5> values);

	int& operator[](std::size_t);
	int operator[](std::size_t) const;

	int& operator[](Role);
	int operator[](Role) const;

	PopulationTable& operator+=(const PopulationTable& other);

	int size() const;
	int size(Role role) const;
	int adults() const;

private:
	std::array<int, 5> table;
};

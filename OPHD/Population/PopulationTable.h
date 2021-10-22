#pragma once

#include <cstddef>


struct PopulationTable
{
	int child{};
	int student{};
	int worker{};
	int scientist{};
	int retiree{};


	int employable() const;
	int adults() const;
	int size() const;

	int& operator[](std::size_t);
	int operator[](std::size_t) const;

	PopulationTable& operator+=(const PopulationTable& other);
	PopulationTable& operator-=(const PopulationTable& other);

	PopulationTable operator/(const PopulationTable& other) const;
	PopulationTable operator%(const PopulationTable& other) const;

	PopulationTable cap(const PopulationTable& other) const;
};

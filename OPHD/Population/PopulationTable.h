#pragma once

#include <array>

struct PopulationTable
{
	int child;
	int student;
	int worker;
	int scientist;
	int retiree;


	int& operator[](std::size_t);
	int operator[](std::size_t) const;

	PopulationTable& operator+=(const PopulationTable& other);
	PopulationTable& operator-=(const PopulationTable& other);

	PopulationTable operator/(const PopulationTable& other) const;
	PopulationTable operator%(const PopulationTable& other) const;

	PopulationTable cap(const PopulationTable& other) const;

	int size() const;
	int adults() const;
};

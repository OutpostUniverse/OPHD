#pragma once

#include <array>

struct PopulationTable
{
	int child;
	int student;
	int worker;
	int scientist;
	int retiree;


	enum class Role
	{
		Child,
		Student,
		Worker,
		Scientist,
		Retired
	};

	int& operator[](std::size_t);
	int operator[](std::size_t) const;

	int& operator[](Role);
	int operator[](Role) const;

	PopulationTable& operator+=(const PopulationTable& other);

	PopulationTable operator/(const PopulationTable& other) const;
	PopulationTable operator%(const PopulationTable& other) const;

	int size() const;
	int adults() const;
};

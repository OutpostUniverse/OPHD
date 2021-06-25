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

	int& operator[](int);
	int& operator[](Role);
	int operator[](Role) const;

	void clear();

	int size();
	int size(Role role);

private:
	std::array<int, 5> table;
};
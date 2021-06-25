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

	int size() const;
	int size(Role role)const ;

private:
	std::array<int, 5> table;
};
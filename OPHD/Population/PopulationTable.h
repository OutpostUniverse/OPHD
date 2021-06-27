#pragma once

#include <array>

struct PopulationTable
{
public:
	enum PersonRole
	{
		ROLE_CHILD,
		ROLE_STUDENT,
		ROLE_WORKER,
		ROLE_SCIENTIST,
		ROLE_RETIRED
	};

	int& operator[](std::size_t);
	int operator[](std::size_t) const;

	int& operator[](PersonRole);
	int operator[](PersonRole) const;

	void clear();

	int size() const;
	int size(PersonRole role)const;

private:
	std::array<int, 5> table;
};

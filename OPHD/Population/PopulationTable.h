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

	int& operator[](int);
	int& operator[](PersonRole);
	int operator[](PersonRole) const;

	void clear();

	int size();
	int size(PersonRole role);

private:
	std::array<int, 5> table;
};
#include "Population.h"

int& PopulationTable::operator[](int index)
{
	return table[index];
}

// Implementation of [] operator.  This function must return a
// reference as array element can be put on left side
int& PopulationTable::operator[](PersonRole role)
{
    return table[role];
}


int PopulationTable::operator[](PersonRole role) const
{
	return table[role];
}


void PopulationTable::clear()
{
    table.fill(0);
}


/**
 * Gets the size of the entire population.
 */
int PopulationTable::size()
{
	int count = 0;
	for (auto& populationRole : table)
	{
		count += populationRole;
	}
	return count;
}


/**
 * Gets the size of a specific segment of the population.
 */
int PopulationTable::size(PersonRole personRole)
{
	return table[personRole];
}

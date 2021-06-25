#include "Population.h"

int& PopulationTable::operator[](std::size_t index)
{
	return table[index];
}

// Implementation of [] operator.  This function must return a
// reference as array element can be put on left side
int& PopulationTable::operator[](Role role)
{
    return table[static_cast<std::size_t>(role)];
}


int PopulationTable::operator[](Role role) const
{
	return table[static_cast<int>(role)];
}


void PopulationTable::clear()
{
    table.fill(0);
}


/**
 * Gets the size of the entire population.
 */
int PopulationTable::size() const
{
	int count = 0;
	for (auto populationRole : table)
	{
		count += populationRole;
	}
	return count;
}


/**
 * Gets the size of a specific segment of the population.
 */
int PopulationTable::size(Role personRole) const
{
	return table[static_cast<std::size_t > (personRole)];
}

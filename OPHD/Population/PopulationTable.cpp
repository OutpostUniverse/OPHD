#include "PopulationTable.h"
#include <numeric>


int& PopulationTable::operator[](std::size_t index)
{
	return table[index];
}


int PopulationTable::operator[](std::size_t index) const
{
	return table[index];
}


int& PopulationTable::operator[](PersonRole role)
{
	return table[static_cast<std::size_t>(role)];
}


int PopulationTable::operator[](PersonRole role) const
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
	return std::accumulate(table.begin(), table.end(), 0);
}


/**
 * Gets the size of a specific segment of the population.
 */
int PopulationTable::size(PersonRole personRole) const
{
	return table[static_cast<std::size_t>(personRole)];
}

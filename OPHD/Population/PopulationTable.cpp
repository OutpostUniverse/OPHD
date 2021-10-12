#include "PopulationTable.h"
#include <numeric>


PopulationTable::PopulationTable(std::array<int, 5> values) :
	table(values)
{
}


int& PopulationTable::operator[](std::size_t index)
{
	return table[index];
}


int PopulationTable::operator[](std::size_t index) const
{
	return table[index];
}


int& PopulationTable::operator[](Role role)
{
	return operator[](static_cast<std::size_t>(role));
}


int PopulationTable::operator[](Role role) const
{
	return operator[](static_cast<std::size_t>(role));
}


PopulationTable& PopulationTable::operator+=(const PopulationTable& other)
{
	for (std::size_t i = 0; i < table.size(); ++i)
	{
		table[i] += other.table[i];
	}
	return *this;
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
int PopulationTable::size(Role personRole) const
{
	return operator[](static_cast<std::size_t>(personRole));
}


int PopulationTable::adults() const
{
	return size() - size(PopulationTable::Role::Child);
}

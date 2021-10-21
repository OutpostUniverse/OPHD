#include "PopulationTable.h"

#include <numeric>
#include <stdexcept>
#include <string>
#include <algorithm>


int PopulationTable::employable() const
{
	return worker + scientist;
}


int PopulationTable::adults() const
{
	return student + worker + scientist + retiree;
}


/**
 * Gets the size of the entire population.
 */
int PopulationTable::size() const
{
	return child + student + worker + scientist + retiree;
}


int& PopulationTable::operator[](std::size_t index)
{
	switch (index)
	{
		case 0:
			return child;
		case 1:
			return student;
		case 2:
			return worker;
		case 3:
			return scientist;
		case 4:
			return retiree;
		default:
			throw std::runtime_error("Invalid index to PopulationTable::operator[]: " + std::to_string(index));
	}
}


int PopulationTable::operator[](std::size_t index) const
{
	switch (index)
	{
		case 0:
			return child;
		case 1:
			return student;
		case 2:
			return worker;
		case 3:
			return scientist;
		case 4:
			return retiree;
		default:
			throw std::runtime_error("Invalid index to PopulationTable::operator[]: " + std::to_string(index));
	}
}


PopulationTable& PopulationTable::operator+=(const PopulationTable& other)
{
	child += other.child;
	student += other.student;
	worker += other.worker;
	scientist += other.scientist;
	retiree += other.retiree;

	return *this;
}


PopulationTable& PopulationTable::operator-=(const PopulationTable& other)
{
	child -= other.child;
	student -= other.student;
	worker -= other.worker;
	scientist -= other.scientist;
	retiree -= other.retiree;

	return *this;
}


PopulationTable PopulationTable::operator/(const PopulationTable& other) const
{
	return {
		child / other.child,
		student / other.student,
		worker / other.worker,
		scientist / other.scientist,
		retiree / other.retiree,
	};
}


PopulationTable PopulationTable::operator%(const PopulationTable& other) const
{
	return {
		child % other.child,
		student % other.student,
		worker % other.worker,
		scientist % other.scientist,
		retiree % other.retiree,
	};
}


PopulationTable PopulationTable::cap(const PopulationTable& other) const
{
	return {
		std::min(child, other.child),
		std::min(student, other.student),
		std::min(worker, other.worker),
		std::min(scientist, other.scientist),
		std::min(retiree, other.retiree),
	};
}

#include "ResourcePool.h"


ResourcePool::ResourcePool() :	_commonMetalsOre(0),
								_commonMineralsOre(0),
								_rareMetalsOre(0),
								_rareMineralsOre(0),
								_commonMetals(0),
								_commonMinerals(0),
								_rareMetals(0),
								_rareMinerals(0),
								_energy(0),
								_food(0)
{}

ResourcePool::ResourcePool(const ResourcePool& _r) : _commonMetalsOre(_r._commonMetalsOre),
													_commonMineralsOre(_r._commonMineralsOre),
													_rareMetalsOre(_r._rareMetalsOre),
													_rareMineralsOre(_r._rareMineralsOre),
													_commonMetals(_r._commonMetals),
													_commonMinerals(_r._commonMinerals),
													_rareMetals(_r._rareMetals),
													_rareMinerals(_r._rareMinerals),
													_energy(_r._energy),
													_food(_r._food)
{}

ResourcePool::~ResourcePool()
{}

ResourcePool& ResourcePool::operator=(const ResourcePool& rhs)
{
	_commonMetalsOre = rhs._commonMetalsOre;
	_commonMineralsOre = rhs._commonMineralsOre;
	_rareMetalsOre = rhs._rareMetalsOre;
	_rareMineralsOre = rhs._rareMineralsOre;

	_commonMetals = rhs._commonMetals;
	_commonMinerals = rhs._commonMinerals;
	_rareMetals = rhs._rareMetals;
	_rareMinerals = rhs._rareMinerals;

	_energy = rhs._energy;
	_food = rhs._food;

	return *this;
}


/**
* Sets all values to 0.
*/
void ResourcePool::clear()
{
	_commonMetalsOre = 0;
	_commonMineralsOre = 0;
	_rareMetalsOre = 0;
	_rareMineralsOre = 0;

	_commonMetals = 0;
	_commonMinerals = 0;
	_rareMetals = 0;
	_rareMinerals = 0;

	_energy = 0;
	_food = 0;
}


ResourcePool& ResourcePool::operator+=(const ResourcePool& rhs)
{
	_commonMetalsOre += rhs._commonMetalsOre;
	_commonMineralsOre += rhs._commonMineralsOre;
	_rareMetalsOre += rhs._rareMetalsOre;
	_rareMineralsOre += rhs._rareMineralsOre;

	_commonMetals += rhs._commonMetals;
	_commonMinerals += rhs._commonMinerals;
	_rareMetals += rhs._rareMetals;
	_rareMinerals += rhs._rareMinerals;

	_energy += rhs._energy;
	_food += rhs._food;

	return *this;
}

ResourcePool& ResourcePool::operator-=(const ResourcePool& rhs)
{
	_commonMetalsOre -= rhs._commonMetalsOre;
	_commonMineralsOre -= rhs._commonMineralsOre;
	_rareMetalsOre -= rhs._rareMetalsOre;
	_rareMineralsOre -= rhs._rareMineralsOre;

	_commonMetals -= rhs._commonMetals;
	_commonMinerals -= rhs._commonMinerals;
	_rareMetals -= rhs._rareMetals;
	_rareMinerals -= rhs._rareMinerals;

	_energy -= rhs._energy;
	_food -= rhs._food;

	return *this;
}


bool operator<(const ResourcePool& lhs, const ResourcePool& rhs)
{
	return (lhs.commonMetalsOre() < rhs.commonMetalsOre() &&
		lhs.commonMineralsOre() < rhs.commonMineralsOre() &&
		lhs.rareMetalsOre() < rhs.rareMetalsOre() &&
		lhs.rareMineralsOre() < rhs.rareMineralsOre() &&

		lhs.commonMetals() < rhs.commonMetals() &&
		lhs.commonMinerals() < rhs.commonMinerals() &&
		lhs.rareMetals() < rhs.rareMetals() &&
		lhs.rareMinerals() < rhs.rareMinerals() &&

		lhs.energy() < rhs.energy() &&
		lhs.food() < rhs.food());
}


bool operator>(const ResourcePool& lhs, const ResourcePool& rhs)
{
	return rhs < lhs;
}


bool operator<=(const ResourcePool& lhs, const ResourcePool& rhs)
{
	return (lhs.commonMetalsOre() <= rhs.commonMetalsOre() &&
		lhs.commonMineralsOre() <= rhs.commonMineralsOre() &&
		lhs.rareMetalsOre() <= rhs.rareMetalsOre() &&
		lhs.rareMineralsOre() <= rhs.rareMineralsOre() &&

		lhs.commonMetals() <= rhs.commonMetals() &&
		lhs.commonMinerals() <= rhs.commonMinerals() &&
		lhs.rareMetals() <= rhs.rareMetals() &&
		lhs.rareMinerals() <= rhs.rareMinerals() &&

		lhs.energy() <= rhs.energy() &&
		lhs.food() <= rhs.food());
}


bool operator>=(const ResourcePool& lhs, const ResourcePool& rhs)
{
	return rhs <= lhs;
}

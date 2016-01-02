#include "ResourcePool.h"


ResourcePool::ResourcePool()
{}

ResourcePool::ResourcePool(const ResourcePool& _r) : _resourceTable(_r._resourceTable)
{}


ResourcePool::~ResourcePool()
{}


ResourcePool& ResourcePool::operator=(const ResourcePool& rhs)
{
	_resourceTable = rhs._resourceTable;
	return *this;
}


/**
* Sets all values to 0.
*/
void ResourcePool::clear()
{
	_resourceTable.clear();
}


ResourcePool& ResourcePool::operator+=(ResourcePool& rhs)
{
	_resourceTable[RESOURCE_COMMON_METALS_ORE] += rhs.commonMetalsOre();
	_resourceTable[RESOURCE_COMMON_MINERALS_ORE] += rhs.commonMineralsOre();
	_resourceTable[RESOURCE_RARE_METALS_ORE] += rhs.rareMetalsOre();
	_resourceTable[RESOURCE_RARE_MINERALS_ORE] += rhs.rareMineralsOre();

	_resourceTable[RESOURCE_COMMON_METALS] += rhs.commonMetals();
	_resourceTable[RESOURCE_COMMON_MINERALS] += rhs.commonMinerals();
	_resourceTable[RESOURCE_RARE_METALS] += rhs.rareMetals();
	_resourceTable[RESOURCE_RARE_MINERALS] += rhs.rareMinerals();

	_resourceTable[RESOURCE_FOOD] += rhs.energy();
	_resourceTable[RESOURCE_ENERGY] += rhs.food();

	return *this;
}

ResourcePool& ResourcePool::operator-=(ResourcePool& rhs)
{
	_resourceTable[RESOURCE_COMMON_METALS_ORE] -= rhs.commonMetalsOre();
	_resourceTable[RESOURCE_COMMON_MINERALS_ORE] -= rhs.commonMineralsOre();
	_resourceTable[RESOURCE_RARE_METALS_ORE] -= rhs.rareMetalsOre();
	_resourceTable[RESOURCE_RARE_MINERALS_ORE] -= rhs.rareMineralsOre();

	_resourceTable[RESOURCE_COMMON_METALS] -= rhs.commonMetals();
	_resourceTable[RESOURCE_COMMON_MINERALS] -= rhs.commonMinerals();
	_resourceTable[RESOURCE_RARE_METALS] -= rhs.rareMetals();
	_resourceTable[RESOURCE_RARE_MINERALS] -= rhs.rareMinerals();

	_resourceTable[RESOURCE_FOOD] -= rhs.energy();
	_resourceTable[RESOURCE_ENERGY] -= rhs.food();

	return *this;
}


double ResourcePool::resource(ResourceType _t)
{
	return _resourceTable[_t];
}


void ResourcePool::resource(ResourceType _t, double _d)
{
	_resourceTable[_t] = _d;
}


double ResourcePool::commonMetalsOre() { return resource(RESOURCE_COMMON_METALS_ORE); }
double ResourcePool::commonMineralsOre() { return resource(RESOURCE_COMMON_MINERALS_ORE); }
double ResourcePool::rareMetalsOre() { return resource(RESOURCE_RARE_METALS_ORE); }
double ResourcePool::rareMineralsOre() { return resource(RESOURCE_RARE_MINERALS_ORE); }

double ResourcePool::commonMetals() { return resource(RESOURCE_COMMON_METALS); }
double ResourcePool::commonMinerals() { return resource(RESOURCE_COMMON_MINERALS); }
double ResourcePool::rareMetals() { return resource(RESOURCE_RARE_METALS); }
double ResourcePool::rareMinerals() { return resource(RESOURCE_RARE_MINERALS); }

double ResourcePool::energy() { return resource(RESOURCE_FOOD); }
double ResourcePool::food() { return resource(RESOURCE_ENERGY); }

void ResourcePool::commonMetalsOre(double _d) { resource(RESOURCE_COMMON_METALS_ORE, _d); }
void ResourcePool::commonMineralsOre(double _d) { resource(RESOURCE_COMMON_MINERALS_ORE, _d); }
void ResourcePool::rareMetalsOre(double _d) { resource(RESOURCE_RARE_METALS_ORE, _d); }
void ResourcePool::rareMineralsOre(double _d) { resource(RESOURCE_RARE_MINERALS_ORE, _d); }

void ResourcePool::commonMetals(double _d) { resource(RESOURCE_COMMON_METALS, _d); }
void ResourcePool::commonMinerals(double _d) { resource(RESOURCE_COMMON_MINERALS, _d); }
void ResourcePool::rareMetals(double _d) { resource(RESOURCE_RARE_METALS, _d); }
void ResourcePool::rareMinerals(double _d) { resource(RESOURCE_RARE_MINERALS, _d); }

void ResourcePool::energy(double _d) { resource(RESOURCE_FOOD, _d); }
void ResourcePool::food(double _d) { resource(RESOURCE_ENERGY, _d); }



bool operator<(ResourcePool& lhs, ResourcePool& rhs)
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


bool operator>(ResourcePool& lhs, ResourcePool& rhs)
{
	return rhs < lhs;
}


bool operator<=(ResourcePool& lhs, ResourcePool& rhs)
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


bool operator>=(ResourcePool& lhs, ResourcePool& rhs)
{
	return rhs <= lhs;
}

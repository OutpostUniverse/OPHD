#include "ResourcePool.h"

#include <iostream>

using namespace std;


ResourcePool::ResourcePool(): _capacity(0)
{}

ResourcePool::ResourcePool(const ResourcePool& _r) : _resourceTable(_r._resourceTable), _capacity(_r._capacity)
{}


ResourcePool::~ResourcePool()
{}


ResourcePool& ResourcePool::operator=(const ResourcePool& rhs)
{
	_resourceTable = rhs._resourceTable;
	_capacity = rhs._capacity;

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
	if (_capacity != 0)
	{
		cout << "ResourcePool::operator+=(): Incorrect use of operator. This ResourcePool has a capacity. Use pushResource() or pushResources() instead." << endl;
		return *this;
	}

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
	if (_capacity != 0)
	{
		cout << "ResourcePool::operator-=(): Incorrect use of operator. This ResourcePool has a capacity. Use pullResource() or pullResources() instead." << endl;
		return *this;
	}

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


int ResourcePool::resource(ResourceType _t)
{
	return _resourceTable[_t];
}


void ResourcePool::resource(ResourceType _t, int _i)
{
	_resourceTable[_t] = _i;
}


int ResourcePool::commonMetalsOre() { return resource(RESOURCE_COMMON_METALS_ORE); }
int ResourcePool::commonMineralsOre() { return resource(RESOURCE_COMMON_MINERALS_ORE); }
int ResourcePool::rareMetalsOre() { return resource(RESOURCE_RARE_METALS_ORE); }
int ResourcePool::rareMineralsOre() { return resource(RESOURCE_RARE_MINERALS_ORE); }

int ResourcePool::commonMetals() { return resource(RESOURCE_COMMON_METALS); }
int ResourcePool::commonMinerals() { return resource(RESOURCE_COMMON_MINERALS); }
int ResourcePool::rareMetals() { return resource(RESOURCE_RARE_METALS); }
int ResourcePool::rareMinerals() { return resource(RESOURCE_RARE_MINERALS); }

int ResourcePool::energy() { return resource(RESOURCE_ENERGY); }
int ResourcePool::food() { return resource(RESOURCE_FOOD); }

void ResourcePool::commonMetalsOre(int _i) { resource(RESOURCE_COMMON_METALS_ORE, _i); }
void ResourcePool::commonMineralsOre(int _i) { resource(RESOURCE_COMMON_MINERALS_ORE, _i); }
void ResourcePool::rareMetalsOre(int _i) { resource(RESOURCE_RARE_METALS_ORE, _i); }
void ResourcePool::rareMineralsOre(int _i) { resource(RESOURCE_RARE_MINERALS_ORE, _i); }

void ResourcePool::commonMetals(int _i) { resource(RESOURCE_COMMON_METALS, _i); }
void ResourcePool::commonMinerals(int _i) { resource(RESOURCE_COMMON_MINERALS, _i); }
void ResourcePool::rareMetals(int _i) { resource(RESOURCE_RARE_METALS, _i); }
void ResourcePool::rareMinerals(int _i) { resource(RESOURCE_RARE_MINERALS, _i); }

void ResourcePool::energy(int _i) { resource(RESOURCE_ENERGY, _i); }
void ResourcePool::food(int _i) { resource(RESOURCE_FOOD, _i); }


/**
 * Returns the current amount of resources in the ResourcePool.
 */
int ResourcePool::currentLevel()
{
	int cc = resource(RESOURCE_COMMON_METALS_ORE);
	cc += resource(RESOURCE_COMMON_MINERALS_ORE);
	cc += resource(RESOURCE_RARE_METALS_ORE);
	cc += resource(RESOURCE_RARE_MINERALS_ORE);

	cc += resource(RESOURCE_COMMON_METALS);
	cc += resource(RESOURCE_COMMON_MINERALS);
	cc += resource(RESOURCE_RARE_METALS);
	cc += resource(RESOURCE_RARE_MINERALS);

	cc += resource(RESOURCE_ENERGY);

	cc += resource(RESOURCE_FOOD);

	return cc;
}


/**
 * Returns the remaining capacity of the ResourcePool.
 */
int ResourcePool::remainingCapacity()
{
	int ret = capacity() - currentLevel();
	return ret;
}


bool ResourcePool::atCapacity()
{
	return remainingCapacity() <= 0;
}


bool ResourcePool::empty()
{
	return currentLevel() == 0;
}


/**
 * Attempt to push the defined amount of a specified resource into a ResourcePool.
 * 
 * \return Returns the remainder of the resource if any.
 */
int ResourcePool::pushResource(ResourceType type, int amount)
{
	if (_capacity == 0)
	{
		cout << "ResourcePool::pushResource(): Incorrect use of operator. This ResourcePool has no capacity. Use operator+=() instead." << endl;
		return 0;
	}

	if (remainingCapacity() <= 0)
	{
		return amount;
	}
	else if (remainingCapacity() >= amount)
	{
		_resourceTable[type] += amount;
		return 0;
	}
	else
	{
		int remainder = amount - remainingCapacity();
		_resourceTable[type] += remainingCapacity();
		return remainder;
	}
}

/**
 * Attempt to pull the defined amount of a specified resource.
 * 
 * \return Returns the amount of resources actually pulled.
 */
int ResourcePool::pullResource(ResourceType type, int amount)
{
	if (_capacity == 0)
	{
		cout << "ResourcePool::pullResource(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << endl;
		return 0;
	}

	return 0;
}

/**
 * Attempts to push all available resources into a ResourcePool.
 * 
 * \param rp The ResourcePool to push resources from.
 * 
 * \return	Returns a reference to a ResourcePool with the remainder of
 *			any resources that couldn't be pushed into the ResourcePool.
 */
void ResourcePool::pushResources(ResourcePool& rp)
{
	if (_capacity == 0)
	{
		cout << "ResourcePool::pushResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator+=() instead." << endl;
		return;
	}
	
	rp.commonMetalsOre(pushResource(RESOURCE_COMMON_METALS_ORE, rp.commonMetalsOre()));
	rp.commonMineralsOre(pushResource(RESOURCE_COMMON_MINERALS_ORE, rp.commonMineralsOre()));
	rp.rareMetalsOre(pushResource(RESOURCE_RARE_METALS_ORE, rp.rareMetalsOre()));
	rp.rareMineralsOre(pushResource(RESOURCE_RARE_MINERALS_ORE, rp.rareMineralsOre()));

	rp.commonMetals(pushResource(RESOURCE_COMMON_METALS, rp.commonMetals()));
	rp.commonMinerals(pushResource(RESOURCE_COMMON_MINERALS, rp.commonMinerals()));
	rp.rareMetals(pushResource(RESOURCE_RARE_METALS, rp.rareMetals()));
	rp.rareMinerals(pushResource(RESOURCE_RARE_MINERALS, rp.rareMinerals()));

	rp.food(pushResource(RESOURCE_FOOD, rp.food()));
	rp.energy(pushResource(RESOURCE_ENERGY, rp.energy()));
}

/**
 * Attempts to pull all available resources from a ResourcePool.
 * 
 * \param rp The ResourcePool to pull resources into.
 */
void ResourcePool::pullResources(ResourcePool& rp)
{
	if (_capacity == 0)
	{
		cout << "ResourcePool::pullResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << endl;
		return;
	}
}

/**
 * Sets the maximum amount of resources the ResourcePool can store.
 */
void ResourcePool::capacity(int _i)
{
	_capacity = _i;

	// Prevent negative values.
	if (_i < 0)
		_capacity = 0;
}


// =======================================================================================================
// = Comparison operators.
// =======================================================================================================
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

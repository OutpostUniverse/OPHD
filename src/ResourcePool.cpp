#include "ResourcePool.h"

#include "Constants.h"

#include <iostream>

using namespace std;
using namespace NAS2D::Xml;


ResourcePool::ResourcePool(int cmo, int cmno, int rmo, int rmno, int cm, int cmn, int rm, int rmn, int f, int e): _capacity(0)
{
	commonMetalsOre(cmo);
	commonMineralsOre(cmno);
	rareMetalsOre(rmo);
	rareMineralsOre(rmno);

	commonMetals(cm);
	commonMinerals(cmn);
	rareMetals(rm);
	rareMinerals(rmn);

	food(f);
	energy(e);
}


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

	_resourceTable[RESOURCE_FOOD] += rhs.food();
	_resourceTable[RESOURCE_ENERGY] += rhs.energy();

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

	_resourceTable[RESOURCE_FOOD] -= rhs.food();
	_resourceTable[RESOURCE_ENERGY] -= rhs.energy();

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
	int cc = 0;
	for (auto it = _resourceTable.begin(); it != _resourceTable.end(); ++it)
	{
		// Food and energy are handled differently than mineral resources.
		if(it->first != ResourcePool::RESOURCE_ENERGY && it->first != ResourcePool::RESOURCE_FOOD)
			cc += it->second;
	}

	return cc;
}


/**
 * Returns the remaining capacity of the ResourcePool.
 */
int ResourcePool::remainingCapacity()
{
	#ifdef _DEBUG
	int ret = capacity() - currentLevel();
	return ret;
	#else
	return capacity() - currentLevel();
	#endif
}


bool ResourcePool::atCapacity()
{
	return remainingCapacity() <= 0;
}


bool ResourcePool::empty()
{
	// energy and food are handled differently than other resources.
	return currentLevel() == 0 && energy() == 0 && food() == 0;
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
		_resourceTable[type] += remainingCapacity();
		return amount - remainingCapacity();
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

	
	if (amount <= _resourceTable[type])
	{
		_resourceTable[type] -= amount;
		return amount;
	}
	else if (amount > _resourceTable[type])
	{
		int ret = _resourceTable[type];
		_resourceTable[type] = 0;
		return ret;
	}

	return 0;
}

/**
 * Attempts to push all available resources into a ResourcePool.
 * 
 * \param rp The ResourcePool to push resources from.
 * 
 * \note	Any resources that can't be fit in ResourcePool are left in
 *			the source ResourcePool.
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
 * \param rp The ResourcePool to pull resources to.
 */
void ResourcePool::pullResources(ResourcePool& _rp)
{
	if (_capacity == 0)
	{
		cout << "ResourcePool::pullResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << endl;
		return;
	}

	// Energy is not part of the capacity check and needs to be transfered first.
	_rp.energy(_rp.energy() + pullResource(RESOURCE_ENERGY, energy()));

	// sanity checks
	if (_rp.atCapacity() || empty())
		return;

	_rp.pushResource(RESOURCE_COMMON_METALS_ORE, pullResource(RESOURCE_COMMON_METALS_ORE, commonMetalsOre()));
	_rp.pushResource(RESOURCE_COMMON_MINERALS_ORE, pullResource(RESOURCE_COMMON_MINERALS_ORE, commonMineralsOre()));
	_rp.pushResource(RESOURCE_RARE_METALS_ORE, pullResource(RESOURCE_RARE_METALS_ORE, rareMetalsOre()));
	_rp.pushResource(RESOURCE_RARE_MINERALS_ORE, pullResource(RESOURCE_RARE_MINERALS_ORE, rareMineralsOre()));

	_rp.pushResource(RESOURCE_COMMON_METALS, pullResource(RESOURCE_COMMON_METALS, commonMetals()));
	_rp.pushResource(RESOURCE_COMMON_MINERALS, pullResource(RESOURCE_COMMON_MINERALS, commonMinerals()));
	_rp.pushResource(RESOURCE_RARE_METALS, pullResource(RESOURCE_RARE_METALS, rareMetals()));
	_rp.pushResource(RESOURCE_RARE_MINERALS, pullResource(RESOURCE_RARE_MINERALS, rareMinerals()));

	_rp.pushResource(RESOURCE_FOOD, pullResource(RESOURCE_FOOD, food()));
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


void ResourcePool::serialize(XmlElement* _ti)
{
	_ti->attribute(constants::SAVE_GAME_COMMON_METAL_ORE, commonMetalsOre());
	_ti->attribute(constants::SAVE_GAME_COMMON_MINERAL_ORE, commonMineralsOre());
	_ti->attribute(constants::SAVE_GAME_RARE_METAL_ORE, rareMetalsOre());
	_ti->attribute(constants::SAVE_GAME_RARE_MINERAL_ORE, rareMineralsOre());

	_ti->attribute(constants::SAVE_GAME_COMMON_METAL, commonMetals());
	_ti->attribute(constants::SAVE_GAME_COMMON_MINERAL, commonMinerals());
	_ti->attribute(constants::SAVE_GAME_RARE_METAL, rareMetals());
	_ti->attribute(constants::SAVE_GAME_RARE_MINERAL, rareMinerals());

	_ti->attribute(constants::SAVE_GAME_ENERGY, energy());
	_ti->attribute(constants::SAVE_GAME_FOOD, food());
}


void ResourcePool::deserialize(XmlElement* _ti)
{
	if (_ti == nullptr)
		return;

	XmlAttribute* attribute = _ti->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SAVE_GAME_COMMON_METAL_ORE) attribute->queryIntValue(_resourceTable[RESOURCE_COMMON_METALS_ORE]);
		else if (attribute->name() == constants::SAVE_GAME_COMMON_MINERAL_ORE) attribute->queryIntValue(_resourceTable[RESOURCE_COMMON_MINERALS_ORE]);
		else if (attribute->name() == constants::SAVE_GAME_RARE_METAL_ORE) attribute->queryIntValue(_resourceTable[RESOURCE_RARE_METALS_ORE]);
		else if (attribute->name() == constants::SAVE_GAME_RARE_MINERAL_ORE) attribute->queryIntValue(_resourceTable[RESOURCE_RARE_MINERALS_ORE]);

		else if (attribute->name() == constants::SAVE_GAME_COMMON_METAL) attribute->queryIntValue(_resourceTable[RESOURCE_COMMON_METALS]);
		else if (attribute->name() == constants::SAVE_GAME_COMMON_MINERAL) attribute->queryIntValue(_resourceTable[RESOURCE_COMMON_MINERALS]);
		else if (attribute->name() == constants::SAVE_GAME_RARE_METAL) attribute->queryIntValue(_resourceTable[RESOURCE_RARE_METALS]);
		else if (attribute->name() == constants::SAVE_GAME_RARE_MINERAL) attribute->queryIntValue(_resourceTable[RESOURCE_RARE_MINERALS]);

		else if (attribute->name() == constants::SAVE_GAME_ENERGY) attribute->queryIntValue(_resourceTable[RESOURCE_ENERGY]);
		else if (attribute->name() == constants::SAVE_GAME_FOOD) attribute->queryIntValue(_resourceTable[RESOURCE_FOOD]);

		attribute = attribute->next();
	}
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

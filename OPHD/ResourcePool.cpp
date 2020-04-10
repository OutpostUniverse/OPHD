// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ResourcePool.h"

#include "Constants.h"

#include <iostream>

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
{
	_resourceTable.fill(0);
}


ResourcePool::ResourcePool(const ResourcePool& rhs) : _capacity(rhs._capacity), _resourceTable(rhs._resourceTable)
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
	_resourceTable.fill(0);
}


/**
 *
 */
ResourcePool& ResourcePool::operator+=(const ResourcePool& rhs)
{
	if (_capacity != 0)
	{
		std::cout << "ResourcePool::operator+=(): Incorrect use of operator. This ResourcePool has a capacity. Use pushResource() or pushResources() instead." << std::endl;
		return *this;
	}

	_resourceTable[ResourceType::RESOURCE_COMMON_METALS_ORE] += rhs.commonMetalsOre();
	_resourceTable[ResourceType::RESOURCE_COMMON_MINERALS_ORE] += rhs.commonMineralsOre();
	_resourceTable[ResourceType::RESOURCE_RARE_METALS_ORE] += rhs.rareMetalsOre();
	_resourceTable[ResourceType::RESOURCE_RARE_MINERALS_ORE] += rhs.rareMineralsOre();

	_resourceTable[ResourceType::RESOURCE_COMMON_METALS] += rhs.commonMetals();
	_resourceTable[ResourceType::RESOURCE_COMMON_MINERALS] += rhs.commonMinerals();
	_resourceTable[ResourceType::RESOURCE_RARE_METALS] += rhs.rareMetals();
	_resourceTable[ResourceType::RESOURCE_RARE_MINERALS] += rhs.rareMinerals();

	_resourceTable[ResourceType::RESOURCE_FOOD] += rhs.food();
	_resourceTable[ResourceType::RESOURCE_ENERGY] += rhs.energy();

	_observerCallback();
	return *this;
}


ResourcePool& ResourcePool::operator-=(const ResourcePool& rhs)
{
	_resourceTable[ResourceType::RESOURCE_COMMON_METALS_ORE] -= rhs.commonMetalsOre();
	_resourceTable[ResourceType::RESOURCE_COMMON_MINERALS_ORE] -= rhs.commonMineralsOre();
	_resourceTable[ResourceType::RESOURCE_RARE_METALS_ORE] -= rhs.rareMetalsOre();
	_resourceTable[ResourceType::RESOURCE_RARE_MINERALS_ORE] -= rhs.rareMineralsOre();

	_resourceTable[ResourceType::RESOURCE_COMMON_METALS] -= rhs.commonMetals();
	_resourceTable[ResourceType::RESOURCE_COMMON_MINERALS] -= rhs.commonMinerals();
	_resourceTable[ResourceType::RESOURCE_RARE_METALS] -= rhs.rareMetals();
	_resourceTable[ResourceType::RESOURCE_RARE_MINERALS] -= rhs.rareMinerals();

	_resourceTable[ResourceType::RESOURCE_FOOD] -= rhs.food();
	_resourceTable[ResourceType::RESOURCE_ENERGY] -= rhs.energy();

	_observerCallback();
	return *this;
}


int ResourcePool::resource(ResourceType type) const
{
	return _resourceTable[type];
}


void ResourcePool::resource(ResourceType type, int amount)
{
	_resourceTable[type] = amount;
	_observerCallback();
}


int ResourcePool::commonMetalsOre() const { return resource(ResourceType::RESOURCE_COMMON_METALS_ORE); }
int ResourcePool::commonMineralsOre() const { return resource(ResourceType::RESOURCE_COMMON_MINERALS_ORE); }
int ResourcePool::rareMetalsOre() const { return resource(ResourceType::RESOURCE_RARE_METALS_ORE); }
int ResourcePool::rareMineralsOre() const { return resource(ResourceType::RESOURCE_RARE_MINERALS_ORE); }

int ResourcePool::commonMetals() const { return resource(ResourceType::RESOURCE_COMMON_METALS); }
int ResourcePool::commonMinerals() const { return resource(ResourceType::RESOURCE_COMMON_MINERALS); }
int ResourcePool::rareMetals() const { return resource(ResourceType::RESOURCE_RARE_METALS); }
int ResourcePool::rareMinerals() const { return resource(ResourceType::RESOURCE_RARE_MINERALS); }

int ResourcePool::energy() const { return resource(ResourceType::RESOURCE_ENERGY); }
int ResourcePool::food() const { return resource(ResourceType::RESOURCE_FOOD); }

void ResourcePool::commonMetalsOre(int amount) { resource(ResourceType::RESOURCE_COMMON_METALS_ORE, amount); }
void ResourcePool::commonMineralsOre(int amount) { resource(ResourceType::RESOURCE_COMMON_MINERALS_ORE, amount); }
void ResourcePool::rareMetalsOre(int amount) { resource(ResourceType::RESOURCE_RARE_METALS_ORE, amount); }
void ResourcePool::rareMineralsOre(int amount) { resource(ResourceType::RESOURCE_RARE_MINERALS_ORE, amount); }

void ResourcePool::commonMetals(int amount) { resource(ResourceType::RESOURCE_COMMON_METALS, amount); }
void ResourcePool::commonMinerals(int amount) { resource(ResourceType::RESOURCE_COMMON_MINERALS, amount); }
void ResourcePool::rareMetals(int amount) { resource(ResourceType::RESOURCE_RARE_METALS, amount); }
void ResourcePool::rareMinerals(int amount) { resource(ResourceType::RESOURCE_RARE_MINERALS, amount); }

void ResourcePool::energy(int amount) { resource(ResourceType::RESOURCE_ENERGY, amount); }
void ResourcePool::food(int amount) { resource(ResourceType::RESOURCE_FOOD, amount); }


/**
 * Returns the current amount of resources in the ResourcePool.
 */
int ResourcePool::currentLevel() const
{
	int cc = 0;
	for (size_t i = 0; i < ResourceType::RESOURCE_COUNT; ++i)
	{
		ResourceType _rt = static_cast<ResourceType>(i);
		if (_rt != ResourceType::RESOURCE_ENERGY && _rt != ResourceType::RESOURCE_FOOD)
			cc += _resourceTable[_rt];
	}

	return cc;
}


/**
 * Returns the remaining capacity of the ResourcePool.
 */
int ResourcePool::remainingCapacity() const
{
	int ret = capacity() - currentLevel();
	return ret;
}


bool ResourcePool::atCapacity() const
{
	return remainingCapacity() <= 0;
}


bool ResourcePool::empty() const
{
	// energy and food are handled differently than other resources.
	return currentLevel() == 0 && energy() == 0 && food() == 0;
}


/**
 * Attempt to push the defined amount of a specified resource into a ResourcePool.
 *
 * \return Returns the remainder of the resource if any.
 */
int ResourcePool::pushResource(ResourceType type, int amount, bool forced)
{
	if(forced)
	{
		_resourceTable[type] += amount;
		_observerCallback();
		return 0;
	}

	if (_capacity == 0)
	{
		std::cout << "ResourcePool::pushResource(): Incorrect use of operator. This ResourcePool has no capacity. Use operator+=() instead." << std::endl;
		return 0;
	}

	if (remainingCapacity() <= 0)
	{
		return amount;
	}
	else if (remainingCapacity() >= amount)
	{
		_resourceTable[type] += amount;
		_observerCallback();
		return 0;
	}
	else
	{
		_resourceTable[type] += remainingCapacity();
		_observerCallback();
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
		std::cout << "ResourcePool::pullResource(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << std::endl;
		return 0;
	}


	if (amount <= _resourceTable[type])
	{
		_resourceTable[type] -= amount;
		_observerCallback();
		return amount;
	}
	else if (amount > _resourceTable[type])
	{
		int ret = _resourceTable[type];
		_resourceTable[type] = 0;
		_observerCallback();
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
		std::cout << "ResourcePool::pushResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator+=() instead." << std::endl;
		return;
	}

	rp.commonMetalsOre(pushResource(ResourceType::RESOURCE_COMMON_METALS_ORE, rp.commonMetalsOre()));
	rp.commonMineralsOre(pushResource(ResourceType::RESOURCE_COMMON_MINERALS_ORE, rp.commonMineralsOre()));
	rp.rareMetalsOre(pushResource(ResourceType::RESOURCE_RARE_METALS_ORE, rp.rareMetalsOre()));
	rp.rareMineralsOre(pushResource(ResourceType::RESOURCE_RARE_MINERALS_ORE, rp.rareMineralsOre()));

	rp.commonMetals(pushResource(ResourceType::RESOURCE_COMMON_METALS, rp.commonMetals()));
	rp.commonMinerals(pushResource(ResourceType::RESOURCE_COMMON_MINERALS, rp.commonMinerals()));
	rp.rareMetals(pushResource(ResourceType::RESOURCE_RARE_METALS, rp.rareMetals()));
	rp.rareMinerals(pushResource(ResourceType::RESOURCE_RARE_MINERALS, rp.rareMinerals()));

	rp.food(pushResource(ResourceType::RESOURCE_FOOD, rp.food()));
	rp.energy(pushResource(ResourceType::RESOURCE_ENERGY, rp.energy()));
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
		std::cout << "ResourcePool::pullResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << std::endl;
		return;
	}

	// Energy is not part of the capacity check and needs to be transfered first.
	_rp.energy(_rp.energy() + pullResource(ResourceType::RESOURCE_ENERGY, energy()));

	// sanity checks
	if (_rp.atCapacity() || empty())
		return;

	_rp.pushResource(ResourceType::RESOURCE_COMMON_METALS_ORE, pullResource(ResourceType::RESOURCE_COMMON_METALS_ORE, commonMetalsOre()));
	_rp.pushResource(ResourceType::RESOURCE_COMMON_MINERALS_ORE, pullResource(ResourceType::RESOURCE_COMMON_MINERALS_ORE, commonMineralsOre()));
	_rp.pushResource(ResourceType::RESOURCE_RARE_METALS_ORE, pullResource(ResourceType::RESOURCE_RARE_METALS_ORE, rareMetalsOre()));
	_rp.pushResource(ResourceType::RESOURCE_RARE_MINERALS_ORE, pullResource(ResourceType::RESOURCE_RARE_MINERALS_ORE, rareMineralsOre()));

	_rp.pushResource(ResourceType::RESOURCE_COMMON_METALS, pullResource(ResourceType::RESOURCE_COMMON_METALS, commonMetals()));
	_rp.pushResource(ResourceType::RESOURCE_COMMON_MINERALS, pullResource(ResourceType::RESOURCE_COMMON_MINERALS, commonMinerals()));
	_rp.pushResource(ResourceType::RESOURCE_RARE_METALS, pullResource(ResourceType::RESOURCE_RARE_METALS, rareMetals()));
	_rp.pushResource(ResourceType::RESOURCE_RARE_MINERALS, pullResource(ResourceType::RESOURCE_RARE_MINERALS, rareMinerals()));

	_rp.pushResource(ResourceType::RESOURCE_FOOD, pullResource(ResourceType::RESOURCE_FOOD, food()));
}

/**
 * Sets the maximum amount of resources the ResourcePool can store.
 */
void ResourcePool::capacity(int newCapacity)
{
	_capacity = newCapacity;

	// Prevent negative values.
	if (newCapacity < 0)
		_capacity = 0;
}


void ResourcePool::serialize(NAS2D::Xml::XmlElement* element)
{
	element->attribute(constants::SAVE_GAME_COMMON_METAL_ORE, commonMetalsOre());
	element->attribute(constants::SAVE_GAME_COMMON_MINERAL_ORE, commonMineralsOre());
	element->attribute(constants::SAVE_GAME_RARE_METAL_ORE, rareMetalsOre());
	element->attribute(constants::SAVE_GAME_RARE_MINERAL_ORE, rareMineralsOre());

	element->attribute(constants::SAVE_GAME_COMMON_METAL, commonMetals());
	element->attribute(constants::SAVE_GAME_COMMON_MINERAL, commonMinerals());
	element->attribute(constants::SAVE_GAME_RARE_METAL, rareMetals());
	element->attribute(constants::SAVE_GAME_RARE_MINERAL, rareMinerals());

	element->attribute(constants::SAVE_GAME_ENERGY, energy());
	element->attribute(constants::SAVE_GAME_FOOD, food());
}


void ResourcePool::deserialize(NAS2D::Xml::XmlElement* element)
{
	/// \todo	This should probably trigger an exception.
	if (element == nullptr) { return; }

	XmlAttribute* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SAVE_GAME_COMMON_METAL_ORE) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_COMMON_METALS_ORE]); }
		else if (attribute->name() == constants::SAVE_GAME_COMMON_MINERAL_ORE){ attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_COMMON_MINERALS_ORE]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_METAL_ORE) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_RARE_METALS_ORE]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_MINERAL_ORE) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_RARE_MINERALS_ORE]); }

		else if (attribute->name() == constants::SAVE_GAME_COMMON_METAL) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_COMMON_METALS]); }
		else if (attribute->name() == constants::SAVE_GAME_COMMON_MINERAL) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_COMMON_MINERALS]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_METAL) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_RARE_METALS]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_MINERAL) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_RARE_MINERALS]); }

		else if (attribute->name() == constants::SAVE_GAME_ENERGY) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_ENERGY]); }
		else if (attribute->name() == constants::SAVE_GAME_FOOD) { attribute->queryIntValue(_resourceTable[ResourceType::RESOURCE_FOOD]); }

		attribute = attribute->next();
	}
}


// =======================================================================================================
// = Comparison operators.
// =======================================================================================================
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

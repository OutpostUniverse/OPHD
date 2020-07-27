// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ResourcePool.h"

#include "Constants.h"

#include <algorithm>
#include <iostream>

using namespace NAS2D::Xml;


ResourcePool::ResourcePool(int cmo, int cmno, int rmo, int rmno, int cm, int cmn, int rm, int rmn, int f, int e): mCapacity(0)
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


ResourcePool::ResourcePool(size_t capacity) : mCapacity(static_cast<int>(capacity))
{}


ResourcePool::ResourcePool(const ResourcePool& rhs) : mCapacity(rhs.mCapacity), mResourceTable(rhs.mResourceTable)
{}


ResourcePool& ResourcePool::operator=(const ResourcePool& rhs)
{
	mResourceTable = rhs.mResourceTable;
	mCapacity = rhs.mCapacity;

	return *this;
}


/**
 * Sets all values to 0.
 */
void ResourcePool::clear()
{
	mResourceTable.clear();
}


/**
 *
 */
ResourcePool& ResourcePool::operator+=(const ResourcePool& rhs)
{
	if (mCapacity != 0)
	{
		std::cout << "ResourcePool::operator+=(): Incorrect use of operator. This ResourcePool has a capacity. Use pushResource() or pushResources() instead." << std::endl;
		return *this;
	}

	mResourceTable[ResourceType::CommonMetalsOre] += rhs.commonMetalsOre();
	mResourceTable[ResourceType::CommonMineralsOre] += rhs.commonMineralsOre();
	mResourceTable[ResourceType::RareMetalsOre] += rhs.rareMetalsOre();
	mResourceTable[ResourceType::RareMineralsOre] += rhs.rareMineralsOre();

	mResourceTable[ResourceType::CommonMetals] += rhs.commonMetals();
	mResourceTable[ResourceType::CommonMinerals] += rhs.commonMinerals();
	mResourceTable[ResourceType::RareMetals] += rhs.rareMetals();
	mResourceTable[ResourceType::RareMinerals] += rhs.rareMinerals();

	mResourceTable[ResourceType::Food] += rhs.food();
	mResourceTable[ResourceType::Energy] += rhs.energy();

	mObserverCallback();
	return *this;
}


ResourcePool& ResourcePool::operator-=(const ResourcePool& rhs)
{
	mResourceTable[ResourceType::CommonMetalsOre] -= rhs.commonMetalsOre();
	mResourceTable[ResourceType::CommonMineralsOre] -= rhs.commonMineralsOre();
	mResourceTable[ResourceType::RareMetalsOre] -= rhs.rareMetalsOre();
	mResourceTable[ResourceType::RareMineralsOre] -= rhs.rareMineralsOre();

	mResourceTable[ResourceType::CommonMetals] -= rhs.commonMetals();
	mResourceTable[ResourceType::CommonMinerals] -= rhs.commonMinerals();
	mResourceTable[ResourceType::RareMetals] -= rhs.rareMetals();
	mResourceTable[ResourceType::RareMinerals] -= rhs.rareMinerals();

	mResourceTable[ResourceType::Food] -= rhs.food();
	mResourceTable[ResourceType::Energy] -= rhs.energy();

	mObserverCallback();
	return *this;
}


int ResourcePool::resource(ResourceType type) const
{
	const auto it = mResourceTable.find(type);
	return it == mResourceTable.end() ? 0 : it->second;
}


void ResourcePool::resource(ResourceType type, int amount)
{
	mResourceTable[type] = amount;
	mObserverCallback();
}


int ResourcePool::commonMetalsOre() const { return resource(ResourceType::CommonMetalsOre); }
int ResourcePool::commonMineralsOre() const { return resource(ResourceType::CommonMineralsOre); }
int ResourcePool::rareMetalsOre() const { return resource(ResourceType::RareMetalsOre); }
int ResourcePool::rareMineralsOre() const { return resource(ResourceType::RareMineralsOre); }

int ResourcePool::commonMetals() const { return resource(ResourceType::CommonMetals); }
int ResourcePool::commonMinerals() const { return resource(ResourceType::CommonMinerals); }
int ResourcePool::rareMetals() const { return resource(ResourceType::RareMetals); }
int ResourcePool::rareMinerals() const { return resource(ResourceType::RareMinerals); }

int ResourcePool::energy() const { return resource(ResourceType::Energy); }
int ResourcePool::food() const { return resource(ResourceType::Food); }

void ResourcePool::commonMetalsOre(int amount) { resource(ResourceType::CommonMetalsOre, amount); }
void ResourcePool::commonMineralsOre(int amount) { resource(ResourceType::CommonMineralsOre, amount); }
void ResourcePool::rareMetalsOre(int amount) { resource(ResourceType::RareMetalsOre, amount); }
void ResourcePool::rareMineralsOre(int amount) { resource(ResourceType::RareMineralsOre, amount); }

void ResourcePool::commonMetals(int amount) { resource(ResourceType::CommonMetals, amount); }
void ResourcePool::commonMinerals(int amount) { resource(ResourceType::CommonMinerals, amount); }
void ResourcePool::rareMetals(int amount) { resource(ResourceType::RareMetals, amount); }
void ResourcePool::rareMinerals(int amount) { resource(ResourceType::RareMinerals, amount); }

void ResourcePool::energy(int amount) { resource(ResourceType::Energy, amount); }
void ResourcePool::food(int amount) { resource(ResourceType::Food, amount); }


/**
 * Returns the current amount of resources in the ResourcePool.
 */
int ResourcePool::currentLevel() const
{
	int cc = 0;
	for (std::size_t i = 0; i < mResourceTable.size(); ++i)
	{
		ResourceType _rt = static_cast<ResourceType>(i);
		if (_rt != ResourceType::Energy && _rt != ResourceType::Food)
		{
			const auto it = mResourceTable.find(_rt);
			if (it != mResourceTable.end()) { cc += it->second; }
		}
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
		mResourceTable[type] += amount;
		mObserverCallback();
		return 0;
	}

	if (mCapacity == 0)
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
		mResourceTable[type] += amount;
		mObserverCallback();
		return 0;
	}
	else
	{
		mResourceTable[type] += remainingCapacity();
		mObserverCallback();
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
	if (mCapacity == 0)
	{
		std::cout << "ResourcePool::pullResource(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << std::endl;
		return 0;
	}


	if (amount <= mResourceTable[type])
	{
		mResourceTable[type] -= amount;
		mObserverCallback();
		return amount;
	}
	else if (amount > mResourceTable[type])
	{
		int ret = mResourceTable[type];
		mResourceTable[type] = 0;
		mObserverCallback();
		return ret;
	}

	return 0;
}

/**
 * Attempts to push all available resources into a ResourcePool.
 *
 * \param resourcePool The ResourcePool to push resources from.
 *
 * \note	Any resources that can't be fit in ResourcePool are left in
 *			the source ResourcePool.
 */
void ResourcePool::pushResources(ResourcePool& resourcePool)
{
	if (mCapacity == 0)
	{
		std::cout << "ResourcePool::pushResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator+=() instead." << std::endl;
		return;
	}

	resourcePool.commonMetalsOre(pushResource(ResourceType::CommonMetalsOre, resourcePool.commonMetalsOre(), false));
	resourcePool.commonMineralsOre(pushResource(ResourceType::CommonMineralsOre, resourcePool.commonMineralsOre(), false));
	resourcePool.rareMetalsOre(pushResource(ResourceType::RareMetalsOre, resourcePool.rareMetalsOre(), false));
	resourcePool.rareMineralsOre(pushResource(ResourceType::RareMineralsOre, resourcePool.rareMineralsOre(), false));

	resourcePool.commonMetals(pushResource(ResourceType::CommonMetals, resourcePool.commonMetals(), false));
	resourcePool.commonMinerals(pushResource(ResourceType::CommonMinerals, resourcePool.commonMinerals(), false));
	resourcePool.rareMetals(pushResource(ResourceType::RareMetals, resourcePool.rareMetals(), false));
	resourcePool.rareMinerals(pushResource(ResourceType::RareMinerals, resourcePool.rareMinerals(), false));

	resourcePool.food(pushResource(ResourceType::Food, resourcePool.food(), false));
	resourcePool.energy(pushResource(ResourceType::Energy, resourcePool.energy(), false));
}

/**
 * Attempts to pull all available resources from a ResourcePool.
 *
 * \param resourcePool The ResourcePool to pull resources to.
 */
void ResourcePool::pullResources(ResourcePool& resourcePool)
{
	if (mCapacity == 0)
	{
		std::cout << "ResourcePool::pullResources(): Incorrect use of operator. This ResourcePool has no capacity. Use operator-=() instead." << std::endl;
		return;
	}

	// Energy is not part of the capacity check and needs to be transfered first.
	resourcePool.energy(resourcePool.energy() + pullResource(ResourceType::Energy, energy()));

	// sanity checks
	if (resourcePool.atCapacity() || empty())
		return;

	resourcePool.pushResource(ResourceType::CommonMetalsOre, pullResource(ResourceType::CommonMetalsOre, commonMetalsOre()), false);
	resourcePool.pushResource(ResourceType::CommonMineralsOre, pullResource(ResourceType::CommonMineralsOre, commonMineralsOre()), false);
	resourcePool.pushResource(ResourceType::RareMetalsOre, pullResource(ResourceType::RareMetalsOre, rareMetalsOre()), false);
	resourcePool.pushResource(ResourceType::RareMineralsOre, pullResource(ResourceType::RareMineralsOre, rareMineralsOre()), false);

	resourcePool.pushResource(ResourceType::CommonMetals, pullResource(ResourceType::CommonMetals, commonMetals()), false);
	resourcePool.pushResource(ResourceType::CommonMinerals, pullResource(ResourceType::CommonMinerals, commonMinerals()), false);
	resourcePool.pushResource(ResourceType::RareMetals, pullResource(ResourceType::RareMetals, rareMetals()), false);
	resourcePool.pushResource(ResourceType::RareMinerals, pullResource(ResourceType::RareMinerals, rareMinerals()), false);

	resourcePool.pushResource(ResourceType::Food, pullResource(ResourceType::Food, food()), false);
}

/**
 * Sets the maximum amount of resources the ResourcePool can store.
 */
void ResourcePool::capacity(int newCapacity)
{
	mCapacity = std::clamp(newCapacity, 0, INT32_MAX);
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
		if (attribute->name() == constants::SAVE_GAME_COMMON_METAL_ORE) { attribute->queryIntValue(mResourceTable[ResourceType::CommonMetalsOre]); }
		else if (attribute->name() == constants::SAVE_GAME_COMMON_MINERAL_ORE){ attribute->queryIntValue(mResourceTable[ResourceType::CommonMineralsOre]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_METAL_ORE) { attribute->queryIntValue(mResourceTable[ResourceType::RareMetalsOre]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_MINERAL_ORE) { attribute->queryIntValue(mResourceTable[ResourceType::RareMineralsOre]); }

		else if (attribute->name() == constants::SAVE_GAME_COMMON_METAL) { attribute->queryIntValue(mResourceTable[ResourceType::CommonMetals]); }
		else if (attribute->name() == constants::SAVE_GAME_COMMON_MINERAL) { attribute->queryIntValue(mResourceTable[ResourceType::CommonMinerals]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_METAL) { attribute->queryIntValue(mResourceTable[ResourceType::RareMetals]); }
		else if (attribute->name() == constants::SAVE_GAME_RARE_MINERAL) { attribute->queryIntValue(mResourceTable[ResourceType::RareMinerals]); }

		else if (attribute->name() == constants::SAVE_GAME_ENERGY) { attribute->queryIntValue(mResourceTable[ResourceType::Energy]); }
		else if (attribute->name() == constants::SAVE_GAME_FOOD) { attribute->queryIntValue(mResourceTable[ResourceType::Food]); }

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

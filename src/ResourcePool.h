#pragma once

#include <map>

#include "NAS2D/NAS2D.h"

/**
 * Pretty much just an easy container for keeping track of resources.
 */
class ResourcePool
{
public:

	enum ResourceType
	{
		RESOURCE_COMMON_METALS_ORE,
		RESOURCE_COMMON_MINERALS_ORE,
		RESOURCE_RARE_METALS_ORE,
		RESOURCE_RARE_MINERALS_ORE,

		RESOURCE_COMMON_METALS,
		RESOURCE_COMMON_MINERALS,
		RESOURCE_RARE_METALS,
		RESOURCE_RARE_MINERALS,

		RESOURCE_FOOD,
		RESOURCE_ENERGY
	};

	ResourcePool(int common_metals_ore, int common_minerals_ore, int rare_metals_ore, int rare_minerals_ore, int common_metals, int common_minerals, int rare_metals, int rare_minerals, int food, int energy);

	ResourcePool();

	ResourcePool(const ResourcePool& _r);
	
	~ResourcePool();

	ResourcePool& operator=(const ResourcePool& rhs);
	ResourcePool& operator+=(ResourcePool& rhs);
	ResourcePool& operator-=(ResourcePool& rhs);

	friend bool operator<(ResourcePool& lhs, ResourcePool& rhs);
	friend bool operator>(ResourcePool& lhs, ResourcePool& rhs);
	friend bool operator<=(ResourcePool& lhs, ResourcePool& rhs);
	friend bool operator>=(ResourcePool& lhs, ResourcePool& rhs);

	/**
	 * Sets all values to 0.
	 */
	void clear();

	int resource(ResourceType _t);
	void resource(ResourceType _t, int _i);

	// GETTERS
	int commonMetalsOre();
	int commonMineralsOre();
	int rareMetalsOre();
	int rareMineralsOre();

	int commonMetals();
	int commonMinerals();
	int rareMetals();
	int rareMinerals();

	int energy();
	int food();

	// SETTERS
	void commonMetalsOre(int _i);
	void commonMineralsOre(int _i);
	void rareMetalsOre(int _i);
	void rareMineralsOre(int _i);

	void commonMetals(int _i);
	void commonMinerals(int _i);
	void rareMetals(int _i);
	void rareMinerals(int _i);

	void energy(int _i);
	void food(int _i);


	int pushResource(ResourceType type, int amount);
	int pullResource(ResourceType type, int amount);

	void pushResources(ResourcePool& rp);
	void pullResources(ResourcePool& rp);

	int capacity() const { return _capacity; }
	void capacity(int _i);

	int currentLevel();
	int remainingCapacity();

	bool atCapacity();

	bool empty();

	void serialize(NAS2D::Xml::XmlElement* _ti);
	void deserialize(NAS2D::Xml::XmlElement* _ti);

private:
	typedef std::map<ResourceType, int> ResourceTable;

	ResourceTable		_resourceTable;

	int					_capacity;			// Maximum available capacity of the ResourcePool.
};

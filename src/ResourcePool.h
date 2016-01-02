#pragma once

#include <map>

/**
* Pretty much just an easy container for keeping track of resources.
* 
* \note	Plain Jane structure. No sanity checking at all.
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

	double resource(ResourceType _t);
	void resource(ResourceType _t, double _d);

	// GETTERS
	double commonMetalsOre();
	double commonMineralsOre();
	double rareMetalsOre();
	double rareMineralsOre();

	double commonMetals();
	double commonMinerals();
	double rareMetals();
	double rareMinerals();

	double energy();
	double food();

	// SETTERS
	void commonMetalsOre(double _d);
	void commonMineralsOre(double _d);
	void rareMetalsOre(double _d);
	void rareMineralsOre(double _d);

	void commonMetals(double _d);
	void commonMinerals(double _d);
	void rareMetals(double _d);
	void rareMinerals(double _d);

	void energy(double _d);
	void food(double _d);

private:
	typedef std::map<ResourceType, double> ResourceTable;

	ResourceTable		_resourceTable;
};

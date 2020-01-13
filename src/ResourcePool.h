#pragma once

#include <array>

#include "NAS2D/Signal.h"
#include "NAS2D/Xml/XmlElement.h"


/**
 * Pretty much just an easy container for keeping track of resources.
 */
class ResourcePool
{
public:
	using Callback = NAS2D::Signals::Signal0<void>;

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
		RESOURCE_ENERGY,

		RESOURCE_COUNT					/**< Number of available resource types. */
	};


public:
	ResourcePool(int common_metals_ore, int common_minerals_ore, int rare_metals_ore, int rare_minerals_ore, int common_metals, int common_minerals, int rare_metals, int rare_minerals, int food, int energy);
	ResourcePool();
	ResourcePool(const ResourcePool& _r);

	~ResourcePool();

	ResourcePool& operator=(const ResourcePool& rhs);
	ResourcePool& operator+=(const ResourcePool& rhs);
	ResourcePool& operator-=(const ResourcePool& rhs);

	friend bool operator<(const ResourcePool& lhs, const ResourcePool& rhs);
	friend bool operator>(const ResourcePool& lhs, const ResourcePool& rhs);
	friend bool operator<=(const ResourcePool& lhs, const ResourcePool& rhs);
	friend bool operator>=(const ResourcePool& lhs, const ResourcePool& rhs);

	/**
	 * Sets all values to 0.
	 */
	void clear();

	int resource(ResourceType _t) const;
	void resource(ResourceType _t, int _i);

	// GETTERS
	int commonMetalsOre() const;
	int commonMineralsOre() const;
	int rareMetalsOre() const;
	int rareMineralsOre() const;

	int commonMetals() const;
	int commonMinerals() const;
	int rareMetals() const;
	int rareMinerals() const;

	int energy() const;
	int food() const;

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


	int pushResource(ResourceType, int, bool forced = true);
	int pullResource(ResourceType, int);

	void pushResources(ResourcePool& rp);
	void pullResources(ResourcePool& rp);

	int capacity() const { return _capacity; }
	void capacity(int _i);

	int currentLevel() const;
	int remainingCapacity() const;

	bool atCapacity() const;

	bool empty() const;

	void serialize(NAS2D::Xml::XmlElement* _ti);
	void deserialize(NAS2D::Xml::XmlElement* _ti);

	Callback& resourceObserver() { return _observerCallback; }

private:
	typedef std::array<int, RESOURCE_COUNT> ResourceTable;

private:
	int					_capacity = 0;			/**< Maximum available capacity of the ResourcePool. */

	ResourceTable		_resourceTable;

	Callback			_observerCallback;
};

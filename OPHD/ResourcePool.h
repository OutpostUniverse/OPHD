#pragma once

#include <NAS2D/Signal.h>
#include <NAS2D/Xml/XmlElement.h>

#include <map>


/**
 * Pretty much just an easy container for keeping track of resources.
 */
class ResourcePool
{
public:
	using Callback = NAS2D::Signals::Signal<>;

	enum class ResourceType
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

		RESOURCE_COUNT /**< Number of available resource types. */
	};


public:
	ResourcePool(int common_metals_ore, int common_minerals_ore, int rare_metals_ore, int rare_minerals_ore, int common_metals, int common_minerals, int rare_metals, int rare_minerals, int food, int energy);
	ResourcePool(size_t capacity);
	ResourcePool();
	ResourcePool(const ResourcePool& rhs);

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

	int resource(ResourceType type) const;
	void resource(ResourceType type, int amount);

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
	void commonMetalsOre(int amount);
	void commonMineralsOre(int amount);
	void rareMetalsOre(int amount);
	void rareMineralsOre(int amount);

	void commonMetals(int amount);
	void commonMinerals(int amount);
	void rareMetals(int amount);
	void rareMinerals(int amount);

	void energy(int amount);
	void food(int amount);


	int pushResource(ResourceType, int, bool);
	int pullResource(ResourceType, int);

	void pushResources(ResourcePool& resourcePool);
	void pullResources(ResourcePool& resourcePool);

	int capacity() const { return _capacity; }
	void capacity(int newCapacity);

	int currentLevel() const;
	int remainingCapacity() const;

	bool atCapacity() const;

	bool empty() const;

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);

	Callback& resourceObserver() { return _observerCallback; }

private:
	int _capacity = 0; /**< Maximum available capacity of the ResourcePool. */

	std::map<ResourceType, int> _resourceTable;

	Callback _observerCallback;
};

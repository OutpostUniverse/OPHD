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
		CommonMetalsOre,
		CommonMineralsOre,
		RareMetalsOre,
		RareMineralsOre,

		CommonMetals,
		CommonMinerals,
		RareMetals,
		RareMinerals,

		Food,
		Energy
	};


public:
	ResourcePool() = default;
	ResourcePool(int common_metals_ore, int common_minerals_ore, int rare_metals_ore, int rare_minerals_ore, int common_metals, int common_minerals, int rare_metals, int rare_minerals, int food, int energy);
	ResourcePool(size_t capacity);
	ResourcePool(const ResourcePool& rhs);

	~ResourcePool() = default;

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

	int capacity() const { return mCapacity; }
	void capacity(int newCapacity);

	int currentLevel() const;
	int remainingCapacity() const;

	bool atCapacity() const;

	bool empty() const;

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);

	Callback& resourceObserver() { return mObserverCallback; }

private:
	int mCapacity = 0;
	std::map<ResourceType, int> mResourceTable;
	Callback mObserverCallback;
};

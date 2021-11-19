#pragma once

#include "Common.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Xml/XmlElement.h>

#include <bitset>


struct StorableResources;


class Mine
{
public:
	enum class OreType
	{
		CommonMetals,
		CommonMinerals,
		RareMetals,
		RareMinerals,
	};

	using MineVein = StorableResources;
	using MineVeins = std::vector<MineVein>;

public:
	Mine();
	Mine(MineProductionRate rate);

	bool active() const;
	void active(bool newActive);

	bool exhausted() const;
	void checkExhausted();

	MineProductionRate productionRate() const { return mProductionRate; }

	int depth() const;
	void increaseDepth();

	StorableResources availableResources() const;
	StorableResources totalYield() const;

	std::bitset<4> miningEnabled() const;
	void miningEnabled(OreType oreType, bool value);

	StorableResources pull(const StorableResources& maxTransfer);

public:
	NAS2D::Xml::XmlElement* serialize(NAS2D::Point<int> location);
	void deserialize(NAS2D::Xml::XmlElement* element);

private:
	Mine(const Mine&) = delete;
	Mine& operator=(const Mine&) = delete;

private:
	MineVeins mVeins; /**< Ore veins */
	MineProductionRate mProductionRate = MineProductionRate::Low; /**< Mine's production rate. */

	/**
	 * Flags indicating several states for the mine:
	 * 
	 * [0] : Mine Common Metal Ore
	 * [1] : Mine Common Mineral Ore
	 * [2] : Mine Rare Metal Ore
	 * [3] : Mine Rare Mineral Ore
	 * [4] : Mine is active
	 * [5] : Mine is exhausted
	 */
	std::bitset<6> mFlags; /**< Set of flags. */
};

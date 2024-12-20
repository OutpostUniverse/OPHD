#pragma once

#include "../Common.h"
#include "../StorableResources.h"

#include <NAS2D/Math/Point.h>

#include <bitset>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}


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

public:
	Mine();
	Mine(MineProductionRate rate);

	bool active() const;
	void active(bool newActive);

	bool exhausted() const;

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
	StorableResources mTappedReserves;
	int mCurrentDepth{0};
	MineProductionRate mProductionRate = MineProductionRate::Low;

	/**
	 * Flags indicating several states for the mine:
	 *
	 * [0] : Mine Common Metal Ore
	 * [1] : Mine Common Mineral Ore
	 * [2] : Mine Rare Metal Ore
	 * [3] : Mine Rare Mineral Ore
	 * [4] : Mine is active
	 */
	std::bitset<5> mFlags; /**< Set of flags. */
};

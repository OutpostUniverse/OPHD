#pragma once

#include "../StorableResources.h"

#include <libOPHD/EnumOreDepositYield.h>

#include <NAS2D/Math/Point.h>

#include <bitset>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}


class OreDeposit
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
	OreDeposit();
	OreDeposit(OreDepositYield yield);

	bool active() const;
	void active(bool newActive);

	bool exhausted() const;

	OreDepositYield productionRate() const { return mProductionRate; }

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
	OreDeposit(const OreDeposit&) = delete;
	OreDeposit& operator=(const OreDeposit&) = delete;

private:
	StorableResources mTappedReserves;
	int mCurrentDepth{0};
	OreDepositYield mProductionRate = OreDepositYield::Low;

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

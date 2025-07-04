#pragma once

#include "../EnumOreDepositYield.h"
#include "../StorableResources.h"

#include <NAS2D/Math/Point.h>


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

	bool exhausted() const;

	OreDepositYield yield() const { return mYield; }

	int depth() const;
	void increaseDepth();

	StorableResources availableResources() const;
	StorableResources totalYield() const;

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
	OreDepositYield mYield = OreDepositYield::Low;
};

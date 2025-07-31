#pragma once

#include "../StorableResources.h"

#include <NAS2D/Math/Point.h>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}


enum class OreDepositYield;


class OreDeposit
{
public:
	OreDeposit();
	OreDeposit(OreDepositYield yield);
	OreDeposit(const StorableResources& availableReserves, OreDepositYield yield, int digDepth);

	int digDepth() const;
	void increaseDepth();

	OreDepositYield yield() const;

	StorableResources totalYield() const;
	StorableResources availableResources() const;
	StorableResources extract(const StorableResources& maxTransfer);

	bool isExhausted() const;

public:
	static NAS2D::Xml::XmlElement* serialize(const OreDeposit& oreDeposit, NAS2D::Point<int> location);
	static OreDeposit deserialize(NAS2D::Xml::XmlElement* element);

private:
	StorableResources mTappedReserves;
	OreDepositYield mYield;
	int mDigDepth{0};
};

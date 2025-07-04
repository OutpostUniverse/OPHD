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

	int depth() const;
	void increaseDepth();

	OreDepositYield yield() const;

	StorableResources totalYield() const;
	StorableResources availableResources() const;
	StorableResources extract(const StorableResources& maxTransfer);

	bool exhausted() const;

public:
	NAS2D::Xml::XmlElement* serialize(NAS2D::Point<int> location);
	void deserialize(NAS2D::Xml::XmlElement* element);

private:
	OreDeposit(const OreDeposit&) = delete;
	OreDeposit& operator=(const OreDeposit&) = delete;

private:
	StorableResources mTappedReserves;
	int mCurrentDepth{0};
	OreDepositYield mYield;
};

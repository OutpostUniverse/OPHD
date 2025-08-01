#pragma once

#include "../StorableResources.h"

#include <NAS2D/Math/Point.h>


enum class OreDepositYield;


class OreDeposit
{
public:
	OreDeposit(OreDepositYield yield, NAS2D::Point<int> location);
	OreDeposit(const StorableResources& availableReserves, OreDepositYield yield, NAS2D::Point<int> location, int digDepth);

	NAS2D::Point<int> location() const;
	int digDepth() const;
	void increaseDepth();

	OreDepositYield yield() const;

	StorableResources totalYield() const;
	StorableResources availableResources() const;
	StorableResources extract(const StorableResources& maxTransfer);

	bool isExhausted() const;

private:
	StorableResources mTappedReserves;
	OreDepositYield mYield;
	NAS2D::Point<int> mLocation;
	int mDigDepth;
};

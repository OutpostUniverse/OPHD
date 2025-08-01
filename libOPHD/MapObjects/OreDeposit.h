#pragma once

#include "../StorableResources.h"


enum class OreDepositYield;


class OreDeposit
{
public:
	OreDeposit(OreDepositYield yield);
	OreDeposit(const StorableResources& availableReserves, OreDepositYield yield, int digDepth);

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
	int mDigDepth{0};
};

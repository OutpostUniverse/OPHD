#include "OreDeposit.h"

#include "../EnumOreDepositYield.h"

#include <algorithm>
#include <array>
#include <map>


namespace
{
	const std::map<OreDepositYield, StorableResources> YieldTable =
	{
		{OreDepositYield::Low, {800, 800, 800, 800}},
		{OreDepositYield::Medium, {1000, 1000, 1000, 1000}},
		{OreDepositYield::High, {1250, 1250, 1250, 1250}}
	};
}


OreDeposit::OreDeposit() :
	mYield{OreDepositYield::Low}
{
}


OreDeposit::OreDeposit(OreDepositYield yield) :
	mYield{yield}
{
}


OreDeposit::OreDeposit(const StorableResources& availableReserves, OreDepositYield yield, int digDepth) :
	mTappedReserves{availableReserves},
	mYield{yield},
	mDigDepth{digDepth}
{
}


/**
 * Increases the depth of the mine.
 *
 * \note	This function only modifies the OreDeposit. It has no knowledge of the
 *			maximum digging depth of a planet and doesn't modify any tiles.
 */
void OreDeposit::increaseDepth()
{
	mDigDepth++;
	mTappedReserves += YieldTable.at(mYield);
}


int OreDeposit::digDepth() const
{
	return mDigDepth;
}


OreDepositYield OreDeposit::yield() const
{
	return mYield;
}


StorableResources OreDeposit::totalYield() const
{
	return YieldTable.at(mYield) * mDigDepth;
}


StorableResources OreDeposit::availableResources() const
{
	return mTappedReserves;
}


/**
 * Extracts the specified quantities of Ore from the OreDeposit. If
 * insufficient ore is available, only pulls what's available.
 */
StorableResources OreDeposit::extract(const StorableResources& maxTransfer)
{
	const auto transferAmount = mTappedReserves.cap(maxTransfer);
	mTappedReserves -= transferAmount;

	return transferAmount;
}


bool OreDeposit::isExhausted() const
{
	return mTappedReserves.isEmpty();
}

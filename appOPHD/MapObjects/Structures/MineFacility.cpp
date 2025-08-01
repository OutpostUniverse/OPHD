#include "MineFacility.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/StorableResources.h>
#include <libOPHD/MapObjects/OreDeposit.h>
#include <libOPHD/MapObjects/StructureType.h>

#include <algorithm>
#include <stdexcept>


MineFacility::MineFacility(Tile& tile) :
	Structure{StructureID::MineFacility, tile},
	mOreDeposit{tile.oreDeposit()}
{
	if (mOreDeposit == nullptr)
	{
		throw std::runtime_error("Mine Facility is located on a Tile with no Ore Deposit.");
	}

	mSprite.play(constants::StructureStateConstruction);
}


void MineFacility::maxDepth(int depth)
{
	mMaxDepth = depth;
}


void MineFacility::activated()
{
	mOreDeposit->increaseDepth();
}


StorableResources MineFacility::maxCapacity() const
{
	const auto oreCapacity = mStructureType.rawOreStorageCapacity;
	return {oreCapacity, oreCapacity, oreCapacity, oreCapacity};
}


StorableResources MineFacility::maxTransferAmounts() const
{
	const auto remainingCapacity = maxCapacity() - production();
	auto maxTransfer = remainingCapacity.cap(constants::BaseMineProductionRate);
	return maxTransfer;
}


void MineFacility::think()
{
	if (forceIdle()) { return; }

	if (mDigTurnsRemaining > 0)
	{
		--mDigTurnsRemaining;

		if (mDigTurnsRemaining == 0)
		{
			mOreDeposit->increaseDepth();
			if (mExtensionCompleteHandler) { mExtensionCompleteHandler(this); }
		}

		return;
	}

	if (isIdle())
	{
		if (storage() < maxCapacity())
		{
			enable();
		}
	}

	if (mOreDeposit->isExhausted())
	{
		idle(IdleReason::MineExhausted);
		return;
	}

	if (storage() >= maxCapacity())
	{
		idle(IdleReason::InternalStorageFull);
		return;
	}

	storage() += mOreDeposit->extract(maxTransferAmounts());
}


bool MineFacility::canExtend() const
{
	return (mOreDeposit->digDepth() < mMaxDepth) && (mDigTurnsRemaining == 0);
}


void MineFacility::extend()
{
	if (!canExtend()) { return; }
	mDigTurnsRemaining = constants::BaseMineExtensionTime;
}


bool MineFacility::extending() const
{
	return mDigTurnsRemaining > 0;
}


int MineFacility::digTimeRemaining() const
{
	return mDigTurnsRemaining;
}


int MineFacility::assignedTrucks() const
{
	return mAssignedTrucks;
}


int MineFacility::maxTruckCount() const
{
	return mMaxTruckCount;
}


void MineFacility::addTruck()
{
	mAssignedTrucks = std::clamp(mAssignedTrucks + 1, 1, mMaxTruckCount);
}


void MineFacility::removeTruck()
{
	mAssignedTrucks = std::clamp(mAssignedTrucks - 1, 1, mMaxTruckCount);
}


OreDeposit& MineFacility::oreDeposit()
{
	return *mOreDeposit;
}


const OreDeposit& MineFacility::oreDeposit() const
{
	return *mOreDeposit;
}


void MineFacility::extensionCompleteHandler(ExtensionCompleteDelegate newExtensionCompleteHandler)
{
	mExtensionCompleteHandler = newExtensionCompleteHandler;
}


void MineFacility::assignedTrucks(int count)
{
	mAssignedTrucks = count;
}


void MineFacility::digTimeRemaining(int count)
{
	mDigTurnsRemaining = count;
}

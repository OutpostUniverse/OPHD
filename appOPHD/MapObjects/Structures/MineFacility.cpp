#include "MineFacility.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"

#include <libOPHD/StorableResources.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <algorithm>
#include <stdexcept>


namespace
{
	const int MineFacilityStorageCapacity = 500;

	const StorableResources MaxCapacity{
		MineFacilityStorageCapacity / 4,
		MineFacilityStorageCapacity / 4,
		MineFacilityStorageCapacity / 4,
		MineFacilityStorageCapacity / 4,
	};
}


MineFacility::MineFacility(Tile* tile) :
	Structure(
		StructureClass::Mine,
		StructureID::SID_MINE_FACILITY
	),
	mOreDeposit(tile->oreDeposit())
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
	mOreDeposit->active(true);
}


StorableResources MineFacility::maxTransferAmounts()
{
	const auto remainingCapacity = MaxCapacity - production();
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
		if (storage() < MaxCapacity)
		{
			enable();
		}
	}

	if (mOreDeposit->exhausted())
	{
		idle(IdleReason::MineExhausted);
		return;
	}

	if (true)
	{
		if (storage() >= MaxCapacity)
		{
			idle(IdleReason::InternalStorageFull);
			return;
		}

		storage() += mOreDeposit->pull(maxTransferAmounts());
	}
	else if (!isIdle())
	{
		idle(IdleReason::MineInactive);
	}
}


bool MineFacility::canExtend() const
{
	return (mOreDeposit->depth() < mMaxDepth) && (mDigTurnsRemaining == 0);
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

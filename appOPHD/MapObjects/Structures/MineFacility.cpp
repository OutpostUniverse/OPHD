#include "MineFacility.h"

#include "../../Map/Tile.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/StorableResources.h>
#include <libOPHD/MapObjects/OreDeposit.h>
#include <libOPHD/MapObjects/StructureType.h>

#include <algorithm>
#include <stdexcept>


namespace
{
	constexpr int BaseMineProductionRate{10};
	constexpr int BaseMineExtensionTime{10};
}


MineFacility::MineFacility(Tile& tile) :
	Structure{StructureID::MineFacility, tile},
	mOreDeposit{tile.oreDeposit()}
{
	if (mOreDeposit == nullptr)
	{
		throw std::runtime_error("Mine Facility is located on a Tile with no Ore Deposit.");
	}
}


void MineFacility::maxDepth(int depth)
{
	mMaxDepth = depth;
}


bool MineFacility::isAtMaxStorageCapacity() const
{
	return storage() >= maxCapacity();
}


StorableResources MineFacility::maxCapacity() const
{
	const auto oreCapacity = rawOreStorageCapacity();
	return {oreCapacity, oreCapacity, oreCapacity, oreCapacity};
}


StorableResources MineFacility::maxTransferAmounts() const
{
	const auto availableCapacity = maxCapacity() - storage();
	auto maxTransfer = availableCapacity.cap(BaseMineProductionRate);
	return maxTransfer;
}


void MineFacility::think()
{
	if (isNew())
	{
		mOreDeposit->increaseDepth();
	}

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
		if (!isAtMaxStorageCapacity())
		{
			enable();
		}
	}

	if (mOreDeposit->isExhausted())
	{
		idle(IdleReason::MineExhausted);
		return;
	}

	if (isAtMaxStorageCapacity())
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
	mDigTurnsRemaining = BaseMineExtensionTime;
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

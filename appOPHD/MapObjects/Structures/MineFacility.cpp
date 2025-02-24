#include "MineFacility.h"

#include "../Mine.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"
#include "../../StorableResources.h"

#include <algorithm>


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


MineFacility::MineFacility(Mine* mine) :
	Structure(
		StructureClass::Mine,
		StructureID::SID_MINE_FACILITY
	),
	mMine(mine)
{
	sprite().play(constants::StructureStateConstruction);
}


void MineFacility::mine(Mine* mine)
{
	mMine = mine;
}


void MineFacility::maxDepth(int depth)
{
	mMaxDepth = depth;
}


void MineFacility::activated()
{
	mMine->increaseDepth();
	mMine->active(true);
}


StorableResources MineFacility::maxTransferAmounts()
{
	const auto remainingCapacity = MaxCapacity - production();
	auto maxTransfer = remainingCapacity.cap(constants::BaseMineProductionRate);

	const auto enabledBits = mMine->miningEnabled();
	for (std::size_t i = 0; i < maxTransfer.resources.size(); ++i)
	{
		if (!enabledBits[i])
		{
			maxTransfer.resources[i] = 0;
		}
	}

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
			mMine->increaseDepth();
			mExtensionComplete(this);
		}

		return;
	}

	if (isIdle() && mMine->active())
	{
		if (storage() < MaxCapacity)
		{
			enable();
		}
	}

	if (mMine->exhausted())
	{
		idle(IdleReason::MineExhausted);
		return;
	}

	if (mMine->active())
	{
		if (storage() >= MaxCapacity)
		{
			idle(IdleReason::InternalStorageFull);
			return;
		}

		storage() += mMine->pull(maxTransferAmounts());
	}
	else if (!isIdle())
	{
		idle(IdleReason::MineInactive);
	}
}


bool MineFacility::canExtend() const
{
	return (mMine->depth() < mMaxDepth) && (mDigTurnsRemaining == 0);
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


Mine* MineFacility::mine()
{
	return mMine;
}


const Mine* MineFacility::mine() const
{
	return mMine;
}


MineFacility::ExtensionCompleteSignal::Source& MineFacility::extensionComplete()
{
	return mExtensionComplete;
}


void MineFacility::assignedTrucks(int count)
{
	mAssignedTrucks = count;
}


void MineFacility::digTimeRemaining(int count)
{
	mDigTurnsRemaining = count;
}

#include "MineFacility.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"


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
		constants::MineFacility,
		"structures/mine_facility.sprite",
		StructureClass::Mine,
		StructureID::SID_MINE_FACILITY
	),
	mMine(mine)
{
	sprite().play(constants::StructureStateConstruction);
	maxAge(1200);
	turnsToBuild(2);
	integrityDecayRate(0);

	requiresCHAP(false);
	selfSustained(true);
	storageCapacity(MineFacilityStorageCapacity);
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

		const auto maxTransfer = maxTransferAmounts();
		const auto ore = mMine->pull(maxTransfer);
		storage() += ore;
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

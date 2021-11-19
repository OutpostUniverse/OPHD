#include "MineFacility.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"


namespace
{
	const int MineFacilityStorageCapacity = 500;
}


/**
 * Computes how many units of ore should be pulled.
 */
static int pullCount(MineFacility* mineFacility, size_t index)
{
	const int storageCapacity = (mineFacility->storageCapacity() / 4);
	const int remainingCapacity = storageCapacity - mineFacility->production().resources[index];

	const int total = std::clamp(remainingCapacity, 0, constants::BaseMineProductionRate);

	return total;
}


MineFacility::MineFacility(Mine* mine) : Structure(constants::MineFacility,
	"structures/mine_facility.sprite",
	StructureClass::Mine,
	StructureID::SID_MINE_FACILITY),
	mMine(mine)
{
	sprite().play(constants::StructureStateConstruction);
	maxAge(1200);
	turnsToBuild(2);
	integrityDecayRate(0);

	requiresCHAP(false);
	selfSustained(true);
	storageCapacity(500);
}


void MineFacility::activated()
{
	mMine->increaseDepth();
	mMine->active(true);
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

	static const StorableResources capacity{ MineFacilityStorageCapacity / 4,
		MineFacilityStorageCapacity / 4,
		MineFacilityStorageCapacity / 4,
		MineFacilityStorageCapacity / 4
	};

	if (isIdle() && mMine->active())
	{
		if (storage() < capacity)
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
		if (storage() >= capacity)
		{
			idle(IdleReason::InternalStorageFull);
			return;
		}

		StorableResources ore;

		const auto enabledBits = mMine->miningEnabled();
		for (std::size_t i = 0; i < ore.resources.size(); ++i)
		{
			if (enabledBits[i])
			{
				ore.resources[i] = mMine->pull(static_cast<Mine::OreType>(i), pullCount(this, i));
			}
		}

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

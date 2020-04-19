// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineFacility.h"

#include "../../Constants.h"

/**
 * Computes how many units of ore should be pulled.
 */
static int push_count(MineFacility* _mf)
{
	int push_count = 0;

	if (_mf->production().remainingCapacity() >= constants::BASE_MINE_PRODUCTION_RATE)
	{
		push_count = constants::BASE_MINE_PRODUCTION_RATE;
	}
	else
	{
		push_count = _mf->production().remainingCapacity();
	}

	return push_count;
}


/**
 * 
 */
MineFacility::MineFacility(Mine* mine) : Structure(constants::MINE_FACILITY, "structures/mine_facility.sprite", StructureClass::CLASS_MINE), mMine(mine)
{
	sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
	maxAge(1200);
	turnsToBuild(2);

	requiresCHAP(false);
	selfSustained(true);

	production().capacity(500);
	storage().capacity(500);
}


/**
 * 
 */
void MineFacility::activated()
{
	mMine->increaseDepth();
	mMine->active(true);
}


/**
 * 
 */
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
		if (!storage().atCapacity())
		{
			enable();
		}
	}

	if (mMine->exhausted())
	{
		idle(IdleReason::IDLE_MINE_EXHAUSTED);
		return;
	}

	if (mMine->active())
	{
		if (storage().atCapacity())
		{
			idle(IdleReason::IDLE_MINE_EXHAUSTED);
			return;
		}

		if (mMine->miningCommonMetals())
		{
			production().pushResource(ResourcePool::ResourceType::RESOURCE_COMMON_METALS_ORE, mMine->pull(Mine::OreType::ORE_COMMON_METALS, push_count(this)), false);
		}
		
		if (mMine->miningCommonMinerals())
		{
			production().pushResource(ResourcePool::ResourceType::RESOURCE_COMMON_MINERALS_ORE, mMine->pull(Mine::OreType::ORE_COMMON_MINERALS, push_count(this)), false);
		}
		
		if (mMine->miningRareMetals())
		{
			production().pushResource(ResourcePool::ResourceType::RESOURCE_RARE_METALS_ORE, mMine->pull(Mine::OreType::ORE_RARE_METALS, push_count(this)), false);
		}
		
		if (mMine->miningRareMinerals())
		{
			production().pushResource(ResourcePool::ResourceType::RESOURCE_RARE_MINERALS_ORE, mMine->pull(Mine::OreType::ORE_RARE_MINERALS, push_count(this)), false);
		}

		storage().pushResources(production());
	}
	else if (!isIdle())
	{
		idle(IdleReason::IDLE_MINE_INACTIVE);
	}
}


/**
 * 
 */
bool MineFacility::canExtend() const
{
	return (mMine->depth() < mMaxDepth) && (mDigTurnsRemaining == 0);
}


/**
 * 
 */
void MineFacility::extend()
{
	if (!canExtend()) { return; }
	mDigTurnsRemaining = constants::BASE_MINE_SHAFT_EXTENSION_TIME;
}


/**
 * 
 */
bool MineFacility::extending() const
{
	return mDigTurnsRemaining > 0;
}


/**
 * 
 */
int MineFacility::digTimeRemaining() const
{
	return mDigTurnsRemaining;
}

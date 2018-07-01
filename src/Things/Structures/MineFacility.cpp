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



MineFacility::MineFacility(Mine* mine) : Structure(constants::MINE_FACILITY, "structures/mine_facility.sprite", CLASS_MINE),
mMine(mine)
{
	sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
	maxAge(1200);
	turnsToBuild(2);

	requiresCHAP(false);
	selfSustained(true);

	production().capacity(500);
	storage().capacity(500);
}


MineFacility::~MineFacility()
{}


void MineFacility::activated()
{
	mMine->increaseDepth();
}


void MineFacility::think()
{
	if (forceIdle()) { return; }

	if (isIdle() && mMine->active())
	{
		if (!storage().atCapacity())
		{
			enable();
		}
	}

	if (mMine->exhausted())
	{
		idle();
		return;
	}

	if (mMine->active())
	{
		if (storage().atCapacity())
		{
			idle();
			return;
		}

		if (mMine->miningCommonMetals())
		{
			production().pushResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, mMine->pull(Mine::ORE_COMMON_METALS, push_count(this)));
		}
		
		if (mMine->miningCommonMinerals())
		{
			production().pushResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, mMine->pull(Mine::ORE_COMMON_MINERALS, push_count(this)));
		}
		
		if (mMine->miningRareMetals())
		{
			production().pushResource(ResourcePool::RESOURCE_RARE_METALS_ORE, mMine->pull(Mine::ORE_RARE_METALS, push_count(this)));
		}
		
		if (mMine->miningRareMinerals())
		{
			production().pushResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, mMine->pull(Mine::ORE_RARE_MINERALS, push_count(this)));
		}

		storage().pushResources(production());
	}
	else if (!isIdle())
	{
		idle();
	}
}

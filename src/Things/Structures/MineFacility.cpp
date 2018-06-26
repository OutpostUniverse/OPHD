// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineFacility.h"

#include "../../Constants.h"


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


void MineFacility::think()
{

	if (isIdle() && mMine->active())
	{
		if (!storage().atCapacity())
			enable();
	}

	if (mMine->active())
	{
		if (storage().atCapacity())
		{
			idle();
			return;
		}

		mMine->update();

		production().pushResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, mMine->commonMetalsRate());
		production().pushResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, mMine->commonMineralsRate());
		production().pushResource(ResourcePool::RESOURCE_RARE_METALS_ORE, mMine->rareMetalsRate());
		production().pushResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, mMine->rareMineralsRate());

		storage().pushResources(production());
	}
	else if (!isIdle())
	{
		idle();
	}
}

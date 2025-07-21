#pragma once

#include "PowerStructure.h"


class SeedPower : public PowerStructure
{
public:
	SeedPower(Tile& tile) :
		PowerStructure{StructureID::SID_SEED_POWER, tile}
	{
	}
};

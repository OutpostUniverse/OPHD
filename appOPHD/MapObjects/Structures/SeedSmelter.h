#pragma once

#include "OreRefining.h"


class SeedSmelter : public OreRefining
{
public:
	SeedSmelter() : OreRefining(
		StructureClass::Smelter,
		StructureID::SID_SEED_SMELTER)
	{
	}
};

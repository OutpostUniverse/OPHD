#pragma once

#include "OreRefining.h"

#include "../../Constants/Strings.h"


class SeedSmelter : public OreRefining
{
public:
	SeedSmelter() : OreRefining(
		StructureClass::Smelter,
		StructureID::SID_SEED_SMELTER)
	{
	}
};

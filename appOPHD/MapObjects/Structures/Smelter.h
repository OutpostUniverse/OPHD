#pragma once

#include "OreRefining.h"


class Smelter : public OreRefining
{
public:
	Smelter() : OreRefining(
		StructureClass::Smelter,
		StructureID::SID_SMELTER)
	{
	}
};

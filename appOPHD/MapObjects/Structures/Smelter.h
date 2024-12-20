#pragma once

#include "OreRefining.h"

#include "../../Constants/Strings.h"


class Smelter : public OreRefining
{
public:
	Smelter() : OreRefining(
		StructureClass::Smelter,
		StructureID::SID_SMELTER)
	{
	}
};

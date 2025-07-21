#pragma once

#include "PowerStructure.h"


class FusionReactor : public PowerStructure
{
public:
	FusionReactor(Tile& tile) :
		PowerStructure{StructureID::SID_FUSION_REACTOR, tile}
	{
	}
};

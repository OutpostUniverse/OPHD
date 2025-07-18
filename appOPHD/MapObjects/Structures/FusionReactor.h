#pragma once

#include "PowerStructure.h"


constexpr int FusionReactorBaseProduction = 1000;


class FusionReactor : public PowerStructure
{
public:
	FusionReactor(Tile& tile) :
		PowerStructure{StructureID::SID_FUSION_REACTOR, tile}
	{
	}

protected:
	int calculateMaxEnergyProduction() const override
	{
		return FusionReactorBaseProduction;
	}
};

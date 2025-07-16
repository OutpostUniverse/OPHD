#pragma once

#include "PowerStructure.h"


constexpr int FusionReactorBaseProduction = 1000;


class FusionReactor : public PowerStructure
{
public:
	FusionReactor() :
		PowerStructure(StructureClass::EnergyProduction, StructureID::SID_FUSION_REACTOR)
	{
	}

protected:
	int calculateMaxEnergyProduction() const override
	{
		return FusionReactorBaseProduction;
	}
};

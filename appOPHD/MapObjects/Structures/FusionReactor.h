#pragma once

#include "PowerStructure.h"

#include "../../Constants/Strings.h"

#include <string>


const int FUSION_REACTOR_BASE_PRODUCUCTION = 1000;


class FusionReactor : public PowerStructure
{
public:
	FusionReactor() : PowerStructure(
		StructureClass::EnergyProduction,
		StructureID::SID_FUSION_REACTOR)
	{
	}

protected:
	int calculateMaxEnergyProduction() override
	{
		return FUSION_REACTOR_BASE_PRODUCUCTION;
	}
};

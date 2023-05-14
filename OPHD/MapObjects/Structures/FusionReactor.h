#pragma once

#include "PowerStructure.h"

#include "../../Constants/Strings.h"

#include <string>


const int FUSION_REACTOR_BASE_PRODUCUCTION = 1000;


class FusionReactor : public PowerStructure
{
public:
	FusionReactor() : PowerStructure(
		"structures/fusion_reactor.sprite",
		StructureClass::EnergyProduction,
		StructureID::SID_FUSION_REACTOR)
	{
		maxAge(1000);
		turnsToBuild(10);
		requiresCHAP(false);
		hasCrime(true);
		integrityDecayRate(2);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn({2, 2, 1, 1});
	}

protected:
	int calculateMaxEnergyProduction() override
	{
		return FUSION_REACTOR_BASE_PRODUCUCTION;
	}
};

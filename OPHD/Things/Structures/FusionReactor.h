#pragma once

#include "PowerStructure.h"

#include "../../Constants.h"
#include <string>

const int FUSION_REACTOR_BASE_PRODUCUCTION = 1000;

class FusionReactor : public PowerStructure
{
public:
	FusionReactor() : PowerStructure(constants::FUSION_REACTOR, "structures/fusion_reactor.sprite", StructureClass::EnergyProduction)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(10);
		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn({ 2, 2, 1, 1 });
	}

protected:
	int calculateMaxEnergyProduction() override
	{
		return FUSION_REACTOR_BASE_PRODUCUCTION;
	}
};

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
		maxAge(1000);
		turnsToBuild(10);
		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().commonMetals(2);
		resourcesIn().commonMinerals(2);
		resourcesIn().rareMinerals(1);
		resourcesIn().rareMetals(1);
	}

	void defineResourceOutput() override
	{
		resourcesOut().energy(calculateEnergyProduced());
	}

protected:
	int calculateMaxEnergyProduction() override
	{
		return FUSION_REACTOR_BASE_PRODUCUCTION;
	}
};

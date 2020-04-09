#pragma once

#include "Structure.h"

#include "../../Constants.h"

class FusionReactor : public Structure
{
public:
	FusionReactor() : Structure(constants::FUSION_REACTOR, "structures/fusion_reactor.sprite", StructureClass::CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
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
		resourcesOut().energy(1000);
	}
};

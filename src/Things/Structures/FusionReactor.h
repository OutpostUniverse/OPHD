#pragma once

#include "Structure.h"

#include "../../Constants.h"

class FusionReactor : public Structure
{
public:
	FusionReactor() : Structure(constants::FUSION_REACTOR, "structures/fusion_reactor.sprite", CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(10);
		requiresCHAP(false);
	}


	virtual ~FusionReactor()
	{}

protected:

	virtual void defineResourceInput()
	{
		resourcesIn().commonMetals(2);
		resourcesIn().commonMinerals(2);
		resourcesIn().rareMinerals(1);
		resourcesIn().rareMetals(1);
	}

	virtual void defineResourceOutput()
	{
		resourcesOut().energy(1000);
	}

private:

};

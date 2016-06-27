#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Residence : public Structure
{
public:
	Residence() : Structure(constants::RESIDENCE, "structures/residential_1.sprite", STRUCTURE_RESIDENCE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(true);

		defineResourceCostToBuild();
	}


	~Residence()
	{}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(25);
		resourcesCostToBuild().rareMetals(2);
		resourcesCostToBuild().commonMinerals(5);
		resourcesCostToBuild().rareMinerals(0);
	}

private:

};

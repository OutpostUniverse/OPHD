#pragma once

#include "Structure.h"

class CHAP : public Structure
{
public:
	CHAP() : Structure(constants::CHAP, "structures/chap.sprite", STRUCTURE_LIFE_SUPPORT)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(5);

		requiresCHAP(false);
		defineResourceCostToBuild();
	}


	~CHAP()
	{}

protected:

	virtual void defineResourceInput()
	{
		resourcesIn().commonMinerals(4);
		resourcesIn().rareMinerals(1);
		resourcesIn().energy(10);
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(50);
		resourcesCostToBuild().rareMetals(20);
		resourcesCostToBuild().commonMinerals(10);
		resourcesCostToBuild().rareMinerals(5);
	}

private:

};



#pragma once

#include "Structure.h"

class CHAP : public Structure
{
public:
	CHAP() : Structure(constants::CHAP, "structures/chap.sprite", CLASS_LIFE_SUPPORT)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(5);

		requiresCHAP(false);
	}


	virtual ~CHAP()
	{}

protected:

	virtual void defineResourceInput()
	{
		resourcesIn().commonMinerals(2);
		resourcesIn().rareMinerals(1);
		resourcesIn().rareMetals(1);
		resourcesIn().energy(10);
	}

	virtual void defineResourceOutput()
	{
	}

private:

};

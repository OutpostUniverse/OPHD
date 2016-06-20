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

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		resourcesValue().commonMetals(20);
		resourcesValue().rareMetals(5);
	}

private:

};



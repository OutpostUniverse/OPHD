#pragma once

#include "Structure.h"

class CHAP : public Structure
{
public:
	CHAP() : Structure(constants::CHAP, "structures/chap.sprite", STRUCTURE_ATMOSPHERE_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(5);

		requiresCHAP(false);
		providesCHAP(true);

		priority(PRIORITY_HIGH);
	}


	~CHAP()
	{}


	void update()
	{
		incrementAge();
	}

protected:

	virtual void defineResourceInput()
	{
		mResourcesInput.commonMinerals(4);
		mResourcesInput.rareMinerals(1);
		mResourcesInput.energy(10);
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals(20);
		mResourceValue.rareMetals(5);
	}

private:

};



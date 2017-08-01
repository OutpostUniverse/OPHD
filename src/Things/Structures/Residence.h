#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Residence : public Structure
{
public:
	Residence() : Structure(constants::RESIDENCE, "structures/residential_1.sprite", CLASS_RESIDENCE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(true);
	}


	virtual ~Residence()
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

private:

};

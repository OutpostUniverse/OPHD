#pragma once

#include "Structure.h"

#include "../../Constants.h"

class University : public Structure
{
public:
	University() : Structure(constants::UNIVERSITY, "structures/university.sprite", STRUCTURE_UNIVERSITY)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}


	virtual ~University()
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

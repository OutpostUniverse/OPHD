#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Nursery : public Structure
{
public:
	Nursery() : Structure(constants::NURSERY, "structures/nursery_01.sprite", CLASS_NURSERY)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:

	virtual void think()
	{}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(2);
	}

	virtual void defineResourceOutput()
	{}

private:

};

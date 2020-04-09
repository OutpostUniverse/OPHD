#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Park : public Structure
{
public:
	Park() : Structure(constants::PARK, "structures/park.sprite", StructureClass::CLASS_PARK)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(1);
	}
};

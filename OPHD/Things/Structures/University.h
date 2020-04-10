#pragma once

#include "Structure.h"

#include "../../Constants.h"

class University : public Structure
{
public:
	University() : Structure(constants::UNIVERSITY, "structures/university.sprite", StructureClass::CLASS_UNIVERSITY)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(1);
	}
};

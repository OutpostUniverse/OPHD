#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Nursery : public Structure
{
public:
	Nursery() : Structure(constants::NURSERY, "structures/nursery_01.sprite", StructureClass::CLASS_NURSERY)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(2);
	}
};

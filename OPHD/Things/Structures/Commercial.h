#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Commercial : public Structure
{
public:
	Commercial() : Structure(constants::COMMERCIAL, "structures/commercial.sprite", StructureClass::Commercial)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(2);
	}
};

#pragma once

#include "Structure.h"

#include "../../Constants.h"

class UndergroundPolice : public Structure
{
public:
	UndergroundPolice() : Structure(constants::UNDERGROUND_POLICE, "structures/police_underground.sprite", StructureClass::UndergroundPolice)
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

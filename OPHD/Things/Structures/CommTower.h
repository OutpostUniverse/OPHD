#pragma once

#include "Structure.h"

#include "../../Constants.h"

class CommTower : public Structure
{
public:

	CommTower() : Structure(constants::COMM_TOWER, "structures/communications_tower.sprite", StructureClass::CLASS_COMM)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(3);
	}
};

#pragma once

#include "Structure.h"

#include "../../Constants.h"

class RecreationCenter : public Structure
{
public:
	RecreationCenter() : Structure(constants::RECREATION_CENTER, "structures/recreation_center.sprite", StructureClass::CLASS_RECREATION_CENTER)
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

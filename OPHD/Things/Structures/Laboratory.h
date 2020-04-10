#pragma once

#include "Structure.h"

class Laboratory : public Structure
{
public:
	Laboratory() : Structure(constants::LABORATORY, "structures/laboratory_underground.sprite", StructureClass::CLASS_LABORATORY)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(1);
	}
};

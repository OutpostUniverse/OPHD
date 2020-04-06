#pragma once

#include "Structure.h"

class HotLaboratory : public Structure
{
public:
	HotLaboratory() : Structure(constants::HOT_LABORATORY, "structures/labo_surface.sprite", CLASS_LABORATORY)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(5);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(3);
	}
};

class Laboratory : public Structure
{
public:
	Laboratory() : Structure(constants::LABORATORY, "structures/laboratory_underground.sprite", CLASS_LABORATORY)
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

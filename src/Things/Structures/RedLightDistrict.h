#pragma once

#include "Structure.h"

#include "../../Constants.h"

class RedLightDistrict : public Structure
{
public:
	RedLightDistrict() : Structure(constants::RED_LIGHT_DISTRICT, "structures/red_light_district.sprite", CLASS_RESIDENCE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	virtual void defineResourceOutput()
	{}

private:

};

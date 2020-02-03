#pragma once

#include "Structure.h"

#include "../../Constants.h"

class SolarPanelArray : public Structure
{
public:
	SolarPanelArray() : Structure(constants::SOLAR_PANEL1, "structures/solar_array1.sprite", CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
	}


	virtual ~SolarPanelArray()
	{}

protected:

	virtual void defineResourceInput()
	{
		// no input it' solar
	}

	virtual void defineResourceOutput()
	{
		resourcesOut().energy(25);
	}

private:

};

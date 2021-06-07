#pragma once

#include "Structure.h"

#include "../../Constants.h"


class UndergroundPolice : public Structure
{
public:
	UndergroundPolice() : Structure(constants::UNDERGROUND_POLICE,
		"structures/police_underground.sprite",
		StructureClass::UndergroundPolice,
		StructureID::SID_UNDERGROUND_POLICE)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}

	int getRange() const
	{
		return 5;
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};

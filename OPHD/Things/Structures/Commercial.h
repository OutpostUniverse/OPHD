#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Commercial : public Structure
{
public:
	Commercial() : Structure(constants::COMMERCIAL,
		"structures/commercial.sprite",
		StructureClass::Commercial,
		StructureTypeID::SID_COMMERCIAL)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(2);
	}
};

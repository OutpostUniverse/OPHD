#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"


class Commercial : public Structure
{
public:
	Commercial() : Structure(constants::Commercial,
		"structures/commercial.sprite",
		StructureClass::Commercial,
		StructureID::SID_COMMERCIAL)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(2);
	}
};

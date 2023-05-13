#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Commercial : public Structure
{
public:
	Commercial() : Structure(
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

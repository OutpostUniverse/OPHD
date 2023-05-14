#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Park : public Structure
{
public:
	Park() : Structure(
		"structures/park.sprite",
		StructureClass::Park,
		StructureID::SID_PARK)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};

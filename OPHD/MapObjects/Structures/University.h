#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class University : public Structure
{
public:
	University() : Structure(constants::University,
		"structures/university.sprite",
		StructureClass::University,
		StructureID::SID_UNIVERSITY)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};

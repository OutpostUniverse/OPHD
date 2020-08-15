#pragma once

#include "Structure.h"

#include "../../Constants.h"

class University : public Structure
{
public:
	University() : Structure(constants::UNIVERSITY,
		"structures/university.sprite",
		StructureClass::University,
		StructureID::SID_UNIVERSITY)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};

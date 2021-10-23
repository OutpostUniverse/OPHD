#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"


class Laboratory : public Structure
{
public:
	Laboratory() : Structure(constants::Laboratory,
		"structures/laboratory_underground.sprite",
		StructureClass::Laboratory,
		StructureID::SID_LABORATORY)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};

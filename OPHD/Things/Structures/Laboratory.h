#pragma once

#include "Structure.h"

#include "../../Constants.h"


class Laboratory : public Structure
{
public:
	Laboratory() : Structure(constants::LABORATORY,
		"structures/laboratory_underground.sprite",
		StructureClass::Laboratory,
		StructureID::SID_LABORATORY)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};

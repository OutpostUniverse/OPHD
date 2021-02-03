#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Recycling : public Structure
{
public:
	Recycling() : Structure(constants::RECYCLING,
		"structures/recycling.sprite",
		StructureClass::Recycling,
		StructureID::SID_RECYCLING)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(4);
	}
};

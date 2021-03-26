#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Nursery : public Structure
{
public:
	Nursery() : Structure(constants::NURSERY,
		"structures/nursery_01.sprite",
		StructureClass::Nursery,
		StructureTypeID::SID_NURSERY)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(2);
	}
};

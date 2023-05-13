#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Nursery : public Structure
{
public:
	Nursery() : Structure(
		StructureClass::Nursery,
		StructureID::SID_NURSERY)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(2);
	}
};

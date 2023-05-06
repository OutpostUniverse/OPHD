#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"


class RecreationCenter : public Structure
{
public:
	RecreationCenter() : Structure(constants::RecreationCenter,
		"structures/recreation_center.sprite",
		StructureClass::RecreationCenter,
		StructureID::SID_RECREATION_CENTER)
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

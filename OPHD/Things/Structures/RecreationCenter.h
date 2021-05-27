#pragma once

#include "Structure.h"

#include "../../Constants.h"


class RecreationCenter : public Structure
{
public:
	RecreationCenter() : Structure(constants::RECREATION_CENTER,
		"structures/recreation_center.sprite",
		StructureClass::RecreationCenter,
		StructureID::SID_RECREATION_CENTER)
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

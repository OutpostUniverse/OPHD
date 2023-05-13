#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Road : public Structure
{
public:
	Road() : Structure(
		StructureClass::Road,
		StructureID::SID_ROAD)
	{
		maxAge(150);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}
};

#pragma once

#include "Structure.h"

#include "../../Constants.h"


class MineShaft : public Structure
{
public:
	MineShaft() : Structure(constants::MINE_SHAFT,
		"structures/mine_shaft.sprite",
		StructureClass::Undefined,
		StructureID::SID_MINE_SHAFT)
	{
		maxAge(1200);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}
};

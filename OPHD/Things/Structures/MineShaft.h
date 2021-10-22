#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"


class MineShaft : public Structure
{
public:
	MineShaft() : Structure(constants::MineShaft,
		"structures/mine_shaft.sprite",
		StructureClass::Undefined,
		StructureID::SID_MINE_SHAFT)
	{
		maxAge(1200);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
		integrityDecayRate(0);
	}
};

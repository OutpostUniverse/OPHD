#pragma once

#include "Structure.h"

#include "../../Constants.h"

#include "../../Mine.h"

class MineShaft : public Structure
{
public:
	MineShaft() : Structure(constants::MINE_SHAFT, "structures/mine_shaft.sprite", StructureClass::Undefined)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1200);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}
};

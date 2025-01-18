#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class MineShaft : public Structure
{
public:
	MineShaft() : Structure(
		StructureClass::Undefined,
		StructureID::SID_MINE_SHAFT)
	{
	}
};

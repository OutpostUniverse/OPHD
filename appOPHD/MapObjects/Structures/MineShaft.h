#pragma once

#include "../Structure.h"


class MineShaft : public Structure
{
public:
	MineShaft(Tile& tile) :
		Structure{StructureID::SID_MINE_SHAFT, tile}
	{
	}
};

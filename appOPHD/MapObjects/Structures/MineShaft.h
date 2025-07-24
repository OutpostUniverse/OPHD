#pragma once

#include "../Structure.h"

#include <libOPHD/EnumStructureID.h>


class MineShaft : public Structure
{
public:
	MineShaft(Tile& tile) :
		Structure{StructureID::MineShaft, tile}
	{
	}
};

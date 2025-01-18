#pragma once

#include "DeployableStructure.h"

#include "OPHD/Map/Tile.h"


class ColonistLander : public DeployableStructure
{
public:
	ColonistLander(Tile& tile) : DeployableStructure(
		StructureClass::Lander,
		StructureID::SID_COLONIST_LANDER,
		tile)
	{}
};

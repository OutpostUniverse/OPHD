#pragma once

#include "DeployableStructure.h"

class Tile;

class ColonistLander : public DeployableStructure
{
public:
	ColonistLander(Tile& tile) : DeployableStructure(
		StructureClass::Lander,
		StructureID::SID_COLONIST_LANDER,
		tile)
	{}
};

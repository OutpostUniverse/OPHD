#pragma once

#include "DeployableStructure.h"

#include "OPHD/Map/Tile.h"


class CargoLander : public DeployableStructure
{
public:
	CargoLander(Tile& tile) : DeployableStructure(
		StructureClass::Lander,
		StructureID::SID_CARGO_LANDER,
		tile)
	{}
};

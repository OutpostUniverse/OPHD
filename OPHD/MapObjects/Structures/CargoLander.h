#pragma once

#include "DeployableStructure.h"

class Tile;

class CargoLander : public DeployableStructure
{
public:
	CargoLander(Tile& tile) : DeployableStructure(
		StructureClass::Lander,
		StructureID::SID_CARGO_LANDER,
		tile)
	{}
};

#pragma once

#include "PowerStructure.h"


class SolarPlant : public PowerStructure
{
public:
	SolarPlant(Tile& tile);

protected:
	int calculateMaxEnergyProduction() const override;
};

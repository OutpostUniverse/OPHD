#pragma once

#include "PowerStructure.h"


class SolarPanelArray : public PowerStructure
{
public:
	SolarPanelArray(Tile& tile);

protected:
	int calculateMaxEnergyProduction() const override;
};

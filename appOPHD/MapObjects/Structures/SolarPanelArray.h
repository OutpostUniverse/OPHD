#pragma once

#include "PowerStructure.h"


class SolarPanelArray : public PowerStructure
{
public:
	SolarPanelArray();

protected:
	int calculateMaxEnergyProduction() const override;
};

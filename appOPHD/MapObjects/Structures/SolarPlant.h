#pragma once

#include "PowerStructure.h"


class SolarPlant : public PowerStructure
{
public:
	SolarPlant();

protected:
	int calculateMaxEnergyProduction() const override;
};

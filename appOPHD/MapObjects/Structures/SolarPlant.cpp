#include "SolarPlant.h"

#include <libOPHD/MeanSolarDistance.h>


namespace
{
	const int SolarPlantBaseProduction = 2000;
}


SolarPlant::SolarPlant() :
	PowerStructure
	{
		StructureClass::EnergyProduction,
		StructureID::SID_SOLAR_PLANT
	}
{
}


int SolarPlant::calculateMaxEnergyProduction() const
{
	return static_cast<int>(SolarPlantBaseProduction / getMeanSolarDistance());
}

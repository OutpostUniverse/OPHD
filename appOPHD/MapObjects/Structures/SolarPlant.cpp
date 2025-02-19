#include "SolarPlant.h"

#include "../../MeanSolarDistance.h"


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


int SolarPlant::calculateMaxEnergyProduction()
{
	return static_cast<int>(SolarPlantBaseProduction / getMeanSolarDistance());
}

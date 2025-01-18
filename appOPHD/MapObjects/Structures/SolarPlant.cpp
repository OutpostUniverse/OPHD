#include "SolarPlant.h"

#include "../../Common.h"
#include "../../Constants/Strings.h"


namespace
{
	const int SOLAR_PLANT_BASE_PRODUCUCTION = 2000;
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
	return static_cast<int>(SOLAR_PLANT_BASE_PRODUCUCTION / getMeanSolarDistance());
}

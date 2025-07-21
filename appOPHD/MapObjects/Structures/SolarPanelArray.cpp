#include "SolarPanelArray.h"

#include <libOPHD/MeanSolarDistance.h>


SolarPanelArray::SolarPanelArray(Tile& tile) :
	PowerStructure{StructureID::SID_SOLAR_PANEL1, tile}
{
}


int SolarPanelArray::calculateMaxEnergyProduction() const
{
	return scaleSolarOutput(PowerStructure::calculateMaxEnergyProduction());
}

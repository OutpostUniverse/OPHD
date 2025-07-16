#include "SolarPanelArray.h"

#include <libOPHD/MeanSolarDistance.h>


namespace
{
	const int SolarPanelBaseProduction = 50;
}


SolarPanelArray::SolarPanelArray(Tile& tile) :
	PowerStructure{StructureClass::EnergyProduction, StructureID::SID_SOLAR_PANEL1, tile}
{
}


int SolarPanelArray::calculateMaxEnergyProduction() const
{
	return static_cast<int>(SolarPanelBaseProduction / getMeanSolarDistance());
}

#include "SolarPanelArray.h"

#include <libOPHD/MeanSolarDistance.h>


namespace
{
	const int SolarPanelBaseProduction = 50;
}


SolarPanelArray::SolarPanelArray() :
	PowerStructure
	{
		StructureClass::EnergyProduction,
		StructureID::SID_SOLAR_PANEL1
	}
{
}


int SolarPanelArray::calculateMaxEnergyProduction() const
{
	return static_cast<int>(SolarPanelBaseProduction / getMeanSolarDistance());
}

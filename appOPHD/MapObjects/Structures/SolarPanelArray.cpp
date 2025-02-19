#include "SolarPanelArray.h"

#include "../../MeanSolarDistance.h"


namespace
{
	const int SOLAR_PANEL_BASE_PRODUCUCTION = 50;
}


SolarPanelArray::SolarPanelArray() :
	PowerStructure
	{
		StructureClass::EnergyProduction,
		StructureID::SID_SOLAR_PANEL1
	}
{
}


int SolarPanelArray::calculateMaxEnergyProduction()
{
	return static_cast<int>(SOLAR_PANEL_BASE_PRODUCUCTION / getMeanSolarDistance());
}

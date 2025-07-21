#include "SolarPlant.h"

#include <libOPHD/MeanSolarDistance.h>


SolarPlant::SolarPlant(Tile& tile) :
	PowerStructure{StructureID::SID_SOLAR_PLANT, tile}
{
}

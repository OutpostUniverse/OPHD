#include "AirShaft.h"

#include <libOPHD/EnumStructureID.h>


AirShaft::AirShaft(Tile& tile) :
	Structure{StructureID::AirShaft, tile}
{
}

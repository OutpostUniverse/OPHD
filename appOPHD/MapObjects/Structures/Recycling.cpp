#include "Recycling.h"

#include <libOPHD/EnumStructureID.h>


Recycling::Recycling(Tile& tile) :
	Structure{StructureID::Recycling, tile}
{
}

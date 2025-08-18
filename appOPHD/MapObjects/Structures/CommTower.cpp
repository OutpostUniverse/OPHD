#include "CommTower.h"

#include <libOPHD/EnumStructureID.h>


CommTower::CommTower(Tile& tile) :
	Structure{StructureID::CommTower, tile}
{
}

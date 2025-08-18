#include "StorageTanks.h"

#include <libOPHD/EnumStructureID.h>


StorageTanks::StorageTanks(Tile& tile) :
	Structure{StructureID::StorageTanks, tile}
{
}

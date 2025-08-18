#include "CommandCenter.h"

#include <libOPHD/EnumStructureID.h>


CommandCenter::CommandCenter(Tile& tile) :
	FoodProduction{StructureID::CommandCenter, tile}
{
}

#include "CommandCenter.h"

#include <libOPHD/EnumStructureID.h>


CommandCenter::CommandCenter(Tile& tile) :
	FoodProduction{StructureID::SID_COMMAND_CENTER, tile}
{
}


int CommandCenter::calculateProduction() const
{
	return foodProduced();
}

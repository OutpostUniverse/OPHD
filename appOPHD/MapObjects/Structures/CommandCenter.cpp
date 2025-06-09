#include "CommandCenter.h"


#include "../../Constants/Numbers.h"


CommandCenter::CommandCenter() : FoodProduction(
	StructureClass::Command,
	StructureID::SID_COMMAND_CENTER)
{
}


int CommandCenter::calculateProduction() const
{
	return foodProduced();
}

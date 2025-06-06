#include "CommandCenter.h"


#include "../../Constants/Numbers.h"


CommandCenter::CommandCenter() : FoodProduction(
	StructureClass::Command,
	StructureID::SID_COMMAND_CENTER)
{
}


int CommandCenter::foodCapacity() const
{
	return constants::BaseStorageCapacity;
}


int CommandCenter::getRange() const
{
	return commRange();
}


int CommandCenter::calculateProduction() const
{
	return 0;
}

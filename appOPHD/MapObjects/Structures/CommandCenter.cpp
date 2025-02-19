#include "CommandCenter.h"


#include "../../Constants/Numbers.h"


CommandCenter::CommandCenter() : FoodProduction(
	StructureClass::Command,
	StructureID::SID_COMMAND_CENTER)
{
}


int CommandCenter::foodCapacity()
{
	return constants::BaseStorageCapacity;
}


int CommandCenter::getRange() const
{
	return operational() ? constants::RobotCommRange : 0;
}


int CommandCenter::calculateProduction()
{
	return 0;
}

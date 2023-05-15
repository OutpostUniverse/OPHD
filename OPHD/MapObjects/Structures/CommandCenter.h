#pragma once

#include "FoodProduction.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"


/**
 * Implements the Command Center structure.
 */
class CommandCenter : public FoodProduction
{
public:
	CommandCenter() : FoodProduction(
		StructureClass::Command,
		StructureID::SID_COMMAND_CENTER)
	{
	}

	int foodCapacity() override
	{
		return constants::BaseStorageCapacity;
	}

	int getRange() const
	{
		return operational() ? constants::RobotCommRange : 0;
	}

protected:
	int calculateProduction() override
	{
		return 0;
	}
};

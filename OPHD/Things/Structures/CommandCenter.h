#pragma once

#include "FoodProduction.h"

/**
 * Implements the Command Center structure.
 */
class CommandCenter: public FoodProduction
{
public:
	CommandCenter(): FoodProduction(constants::COMMAND_CENTER,
		"structures/command_center.sprite",
		StructureClass::Command,
		StructureID::SID_COMMAND_CENTER)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
		selfSustained(true);

		storageCapacity(constants::BASE_STORAGE_CAPACITY);
	}

	int foodCapacity() override
	{
		return constants::BASE_STORAGE_CAPACITY;
	}

protected:
	int calculateProduction() override
	{
		return 0;
	}
};

#pragma once

#include "Structure.h"

/**
 * Implements the Command Center structure.
 */
class CommandCenter: public Structure
{
public:
	CommandCenter(): Structure(constants::COMMAND_CENTER, "structures/command_center.sprite", CLASS_COMMAND)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
		selfSustained(true);
	}
};

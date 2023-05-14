#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"

#include <vector>


class Robot;


/**
 * Implements the Robot Command structure.
 */
class RobotCommand : public Structure
{
public:
	RobotCommand() : Structure(
		"structures/robot_control.sprite",
		StructureClass::RobotCommand,
		StructureID::SID_ROBOT_COMMAND)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(false);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};

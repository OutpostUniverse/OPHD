#pragma once

#include "Structure.h"
#include "../../Constants.h"

#include <vector>


class Robot;


/**
 * Implements the Robot Command structure.
 */
class RobotCommand : public Structure
{
public:
	RobotCommand() : Structure(constants::ROBOT_COMMAND,
		"structures/robot_control.sprite",
		StructureClass::RobotCommand,
		StructureTypeID::SID_ROBOT_COMMAND)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(false);
	}

	bool isControlling(Robot* robot) const;

	bool commandCapacityAvailable() const;
	void addRobot(Robot* robot);
	void removeRobot(Robot* robot);

	const std::vector<Robot*>& robots() { return mRobotList; }

protected:
	void defineResourceInput() override
	{
		energyRequired(5);
	}

private:
	std::vector<Robot*> mRobotList;
};

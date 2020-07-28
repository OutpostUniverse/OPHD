#pragma once

#include "Structure.h"

#include "../Robots/Robot.h"

#include "../../Constants.h"

#include <vector>


/**
 * Implements the Robot Command structure.
 */
class RobotCommand : public Structure
{
public:
	RobotCommand() : Structure(constants::ROBOT_COMMAND, "structures/robot_control.sprite", StructureClass::RobotCommand)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(false);
	}

	bool commandedByThis(Robot* robot) const;

	bool commandCapacityAvailable() const;
	void addRobot(Robot* robot);
	void removeRobot(Robot* robot);

	const RobotList& robots() { return mRobotList; }

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(5);
	}

private:
	RobotList mRobotList;
};

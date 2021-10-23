#include "RobotCommand.h"

#include "../Robots/Robot.h"
#include "../../Constants/Numbers.h"

#include <string>
#include <stdexcept>
#include <algorithm>


/**
 * Gets whether the command facility has additional command capacity remaining.
 */
bool RobotCommand::commandCapacityAvailable() const
{
	return mRobotList.size() < constants::RobotCommandCapacity;
}


/**
 * \param	robot	Pointer to a Robot.
 */
bool RobotCommand::isControlling(Robot* robot) const
{
	return find(mRobotList.begin(), mRobotList.end(), robot) != mRobotList.end();
}


/**
 * Adds a robot to the management pool of the Robot Command structure.
 */
void RobotCommand::addRobot(Robot* robot)
{
	if (mRobotList.size() >= constants::RobotCommandCapacity)
	{
		throw std::runtime_error("RobotCommand::addRobot(): Facility is already at capacity.");
	}

	if (isControlling(robot))
	{
		throw std::runtime_error("RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility. Robot name: " + robot->name());
	}

	mRobotList.push_back(robot);
}


/**
 * Removes a robot from the management pool of the Robot Command structure.
 */
void RobotCommand::removeRobot(Robot* robot)
{
	mRobotList.erase(std::remove(mRobotList.begin(), mRobotList.end(), robot), mRobotList.end());
}

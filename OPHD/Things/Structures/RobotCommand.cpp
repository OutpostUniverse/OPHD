#include "RobotCommand.h"

#include "../Robots/Robot.h"
#include "../../Constants.h"

#include <string>
#include <stdexcept>
#include <algorithm>


/**
 * Gets whether the command facility has additional command capacity remaining.
 */
bool RobotCommand::commandCapacityAvailable() const
{
	return mRobotList.size() < constants::ROBOT_COMMAND_CAPACITY;
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
 * 
 * \param	robot	Pointer to a Robot to add to command list.
 * 
 * \note	Performs some basic sanity checking. Will throw if list is full or
 *			the given pointer to a Robot is already in the list.
 */
void RobotCommand::addRobot(Robot* robot)
{
	if (mRobotList.size() >= constants::ROBOT_COMMAND_CAPACITY)
	{
		throw std::runtime_error("RobotCommand::addRobot(): Facility is already at capacity.");
	}

	if (isControlling(robot))
	{
		const auto message = "RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility. Robot name: " + robot->name();
		std::cout << message << std::endl;
		throw std::runtime_error(message);
	}

	mRobotList.push_back(robot);
}


/**
 * Removes a robot from the management pool of the Robot Command structure.
 */
void RobotCommand::removeRobot(Robot* robot)
{
	if (!robot) { throw std::runtime_error("RobotCommand::removeRobot() called with nullptr"); }
	if (mRobotList.empty()) { return; }

	auto iter = find(mRobotList.begin(), mRobotList.end(), robot);
	if (iter == mRobotList.end()) { return; }

	mRobotList.erase(iter);
}

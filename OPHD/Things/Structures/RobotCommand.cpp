// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
		const auto message = "RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility. Robot name: " + robot->name() + " ID: " + std::to_string(robot->id());
		std::cout << message << std::endl;
		throw std::runtime_error(message);
	}

	mRobotList.push_back(robot);
}


/**
 * Removes a robot to the management pool of the Robot Command structure.
 *
 * \param	robot	Pointer to a Robot to remove from command list.
 */
void RobotCommand::removeRobot(Robot* robot)
{
	if (mRobotList.empty())
	{
		//throw std::runtime_error("RobotCommand::removeRobot(): Robot list empty.");
		return;
	}

	auto _it = find(mRobotList.begin(), mRobotList.end(), robot);
	if (_it == mRobotList.end())
	{
		//throw std::runtime_error("RobotCommand::removeRobot(): Removing a robot that is not under the command of this Robot Command Facility.");
		return;
	}

	mRobotList.erase(_it);
}

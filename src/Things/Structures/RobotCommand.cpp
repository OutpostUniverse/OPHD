// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "RobotCommand.h"

#include "../../Constants.h"

#include <algorithm>


/**
 * Gets whether the command facility has additional command capacity remaining.
 */
bool RobotCommand::commandCapacityAvailable() const
{
	return mRobotList.size() < constants::ROBOT_COMMAND_CAPACITY;
}


/**
 * \param	_r	Pointer to a Robot.
 */
bool RobotCommand::commandedByThis(Robot* _r) const
{
	return find(mRobotList.begin(), mRobotList.end(), _r) != mRobotList.end();
}


/**
 * Adds a robot to the management pool of the Robot Command structure.
 * 
 * \param	_r	Pointer to a Robot to add to command list.
 * 
 * \note	Performs some basic sanity checking. Will throw if list is full or
 *			the given pointer to a Robot is already in the list.
 */
void RobotCommand::addRobot(Robot* _r)
{
	if (mRobotList.size() >= constants::ROBOT_COMMAND_CAPACITY)
	{
		throw std::runtime_error("RobotCommand::addRobot(): Facility is already at capacity.");
	}

	auto _it = find(mRobotList.begin(), mRobotList.end(), _r);
	if (_it != mRobotList.end())
	{
		throw std::runtime_error("RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility.");
	}

	mRobotList.push_back(_r);
}


/**
 * Removes a robot to the management pool of the Robot Command structure.
 *
 * \param	_r	Pointer to a Robot to remove from command list.
 *
 * \note	Performs some basic sanity checking. Will throw if list is empty or
 *			the given pointer to a Robot is not in the list.
 */
void RobotCommand::removeRobot(Robot* _r)
{
	if (mRobotList.empty())
	{
		throw std::runtime_error("RobotCommand::removeRobot(): Robot list empty.");
	}

	auto _it = find(mRobotList.begin(), mRobotList.end(), _r);
	if (_it == mRobotList.end())
	{
		throw std::runtime_error("RobotCommand::removeRobot(): Removing a robot that is not under the command of this Robot Command Facility.");
	}

	mRobotList.erase(_it);
}

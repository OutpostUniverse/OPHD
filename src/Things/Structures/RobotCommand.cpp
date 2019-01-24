// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "RobotCommand.h"

#include "../../Constants.h"

#include <algorithm>

extern int ROBOT_ID_COUNTER; /// \fixme Kludge

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

	if (commandedByThis(_r))
	{
		std::cout << "RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility. CURRENT ID: " << ROBOT_ID_COUNTER << std::endl;
		std::cout << "RCC:ADD: _r addr: " << _r << " name: " << _r->name() << " id: " << _r->id() << std::endl;
		doAlertMessage("Invalid Robot Command", "Robot Command Center: Requested add of a robot already commanded by this RCC. Please submit log to developer and any steps to reproduce.");
		return;
		//throw std::runtime_error("RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility.");
	}

	mRobotList.push_back(_r);
}


/**
 * Removes a robot to the management pool of the Robot Command structure.
 *
 * \param	_r	Pointer to a Robot to remove from command list.
 */
void RobotCommand::removeRobot(Robot* _r)
{
	if (mRobotList.empty())
	{
		//throw std::runtime_error("RobotCommand::removeRobot(): Robot list empty.");
		return;
	}

	auto _it = find(mRobotList.begin(), mRobotList.end(), _r);
	if (_it == mRobotList.end())
	{
		//throw std::runtime_error("RobotCommand::removeRobot(): Removing a robot that is not under the command of this Robot Command Facility.");
		return;
	}

	mRobotList.erase(_it);
}

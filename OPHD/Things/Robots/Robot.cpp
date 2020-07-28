// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Robot.h"

Robot::Robot(const std::string& name, const std::string& sprite_path) :
	Thing(name, sprite_path, "running")
{}


Robot::Robot(const std::string& name, const std::string& sprite_path, const std::string& initialAction) :
	Thing(name, sprite_path, initialAction)
{}


void Robot::startTask(int turns)
{
	if (turns < 1) { throw std::runtime_error("Robot::startTask() called with a value less than 1."); }
	mTurnsToCompleteTask = turns;
}


void Robot::updateTask()
{
	if (mSelfDestruct)
	{
		mSelfDestructCallback();
		die();
	}

	mTurnsToCompleteTask--;

	if (mTurnsToCompleteTask == 0)
	{
		mTaskCompleteCallback(this);
	}

	mFuelCellAge++;

	if (mFuelCellAge == 200)
	{
		die();
	}
}

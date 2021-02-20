#include "Robot.h"

Robot::Robot(const std::string& name, const std::string& sprite_path, Type t) :
	Thing(name, sprite_path, "running"),
	mType{ t }
{}


Robot::Robot(const std::string& name, const std::string& sprite_path, const std::string& initialAction, Type t) :
	Thing(name, sprite_path, initialAction),
	mType{ t }
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

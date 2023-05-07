#include "Robot.h"

Robot::Robot(const std::string& name, const std::string& spritePath, Type type) :
	MapObject(name, spritePath, "running"),
	mType{type}
{}


Robot::Robot(const std::string& name, const std::string& spritePath, const std::string& initialAction, Type type) :
	MapObject(name, spritePath, initialAction),
	mType{type}
{}


void Robot::startTask(int turns)
{
	if (turns < 1) { throw std::runtime_error("Robot::startTask() called with a value less than 1."); }
	mTurnsToCompleteTask = turns;
}


NAS2D::Dictionary Robot::getDataDict() const
{
	return {{
		{"type", static_cast<int>(mType)},
		{"age", mFuelCellAge},
		{"production", mTurnsToCompleteTask},
	}};
}


void Robot::updateTask()
{
	if (mSelfDestruct)
	{
		die();
	}

	if (mCancelTask)
	{
		mTurnsToCompleteTask = 0;
		return;
	}

	mTurnsToCompleteTask--;

	if (mTurnsToCompleteTask == 0)
	{
		mTaskCompleteSignal(this);
	}

	mFuelCellAge++;

	if (mFuelCellAge == 200)
	{
		die();
	}
}

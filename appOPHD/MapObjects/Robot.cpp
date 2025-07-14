#include "Robot.h"

#include "../Constants/Numbers.h"
#include "../Map/Tile.h"

#include <NAS2D/Dictionary.h>


namespace
{
	const std::map<RobotType, int> basicTaskTime{
		{RobotType::Dozer, 0},
		{RobotType::Digger, constants::DiggerTaskTime},
		{RobotType::Miner, constants::MinerTaskTime},
	};

	int getTaskTime(RobotType type, Tile& tile)
	{
		return std::max(1, basicTaskTime.at(type) + static_cast<int>(tile.index()));
	}
}


Robot::Robot(const std::string& name, const std::string& spritePath, RobotType type) :
	MapObject(spritePath, "running"),
	mName(name),
	mType{type}
{}


const std::string& Robot::name() const
{
	return mName;
}


void Robot::die()
{
	mIsDead = true;
}


/// Robot is dead and should be cleaned up.
bool Robot::isDead() const
{
	return mIsDead;
}


void Robot::startTask(Tile& tile)
{
	startTask(getTaskTime(mType, tile));
}


void Robot::startTask(int turns)
{
	if (turns < 1) { throw std::runtime_error("Robot task time must be at least 1 turn"); }
	mTurnsToCompleteTask = turns;
}


void Robot::taskCompleteHandler(TaskCompleteDelegate newTaskCompleteHandler)
{
	mTaskCompleteHandler = newTaskCompleteHandler;
}


NAS2D::Dictionary Robot::getDataDict() const
{
	return {{
		{"type", static_cast<int>(mType)},
		{"age", mFuelCellAge},
		{"production", mTurnsToCompleteTask},
	}};
}


void Robot::processTurn(TileMap& tileMap)
{
	if (mSelfDestruct)
	{
		die();
		return;
	}

	if (mCancelTask)
	{
		mTurnsToCompleteTask = 0;
		return;
	}

	mTurnsToCompleteTask--;

	if (mTurnsToCompleteTask == 0)
	{
		onTaskComplete(tileMap);
		if (mTaskCompleteHandler) { mTaskCompleteHandler(*this); }
	}

	mFuelCellAge++;

	if (mFuelCellAge == 200)
	{
		die();
	}
}

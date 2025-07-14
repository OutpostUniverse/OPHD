#include "Robot.h"

#include "RobotType.h"
#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"
#include "../Map/Tile.h"

#include <NAS2D/Dictionary.h>

#include <array>


namespace
{
	const std::array robotTypes{
		RobotType{constants::Robodigger, "robots/robodigger.sprite"},
		RobotType{constants::Robodozer, "robots/robodozer.sprite"},
		RobotType{constants::Robominer, "robots/robominer.sprite"},
	};

	const std::map<RobotTypeIndex, int> basicTaskTime{
		{RobotTypeIndex::Dozer, 0},
		{RobotTypeIndex::Digger, constants::DiggerTaskTime},
		{RobotTypeIndex::Miner, constants::MinerTaskTime},
	};

	int getTaskTime(RobotTypeIndex robotTypeIndex, Tile& tile)
	{
		return std::max(1, basicTaskTime.at(robotTypeIndex) + static_cast<int>(tile.index()));
	}
}


const RobotType& Robot::robotType(RobotTypeIndex robotTypeIndex)
{
	return robotTypes.at(static_cast<std::size_t>(robotTypeIndex));
}


Robot::Robot(RobotTypeIndex robotTypeIndex) :
	Robot{robotTypeIndex, robotType(robotTypeIndex)}
{}


Robot::Robot(RobotTypeIndex robotTypeIndex, const RobotType& robotType) :
	MapObject(robotType.spritePath, "running"),
	mRobotType{robotType},
	mRobotTypeIndex{robotTypeIndex}
{}


const RobotType& Robot::robotType() const
{
	return mRobotType;
}


const std::string& Robot::name() const
{
	return mRobotType.name;
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
	startTask(getTaskTime(mRobotTypeIndex, tile));
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
		{"type", static_cast<int>(mRobotTypeIndex)},
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

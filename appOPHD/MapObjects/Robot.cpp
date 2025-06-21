#include "Robot.h"

#include "../Constants/Numbers.h"
#include "../Map/Tile.h"


namespace
{
	const std::map<Robot::Type, int> basicTaskTime{
		{Robot::Type::Dozer, 0},
		{Robot::Type::Digger, constants::DiggerTaskTime},
		{Robot::Type::Miner, constants::MinerTaskTime},
	};

	int getTaskTime(Robot::Type type, Tile& tile)
	{
		return std::max(1, basicTaskTime.at(type) + static_cast<int>(tile.index()));
	}
}


Robot::Robot(const std::string& name, const std::string& spritePath, Type type) :
	MapObject(spritePath, "running"),
	mName(name),
	mType{type}
{}


Robot::Robot(const std::string& name, const std::string& spritePath, const std::string& initialAction, Type type) :
	MapObject(spritePath, initialAction),
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


NAS2D::Dictionary Robot::getDataDict() const
{
	return {{
		{"type", static_cast<int>(mType)},
		{"age", mFuelCellAge},
		{"production", mTurnsToCompleteTask},
	}};
}


void Robot::processTurn()
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
		mTaskCompleteSignal(*this);
	}

	mFuelCellAge++;

	if (mFuelCellAge == 200)
	{
		die();
	}
}

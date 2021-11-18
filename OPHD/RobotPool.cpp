#include "RobotPool.h"
#include "RobotPoolHelper.h"
#include "Map/Tile.h"

#include <algorithm>


RobotPool::RobotPool()
{}


/**
 * Frees all resources from the robot pool so this class should be
 * freed after all pointers to robots attained from the pool have been
 * released.
 */
RobotPool::~RobotPool()
{
	clear();
}


void RobotPool::clear()
{
	clearRobots(mDiggers);
	clearRobots(mDozers);
	clearRobots(mMiners);
	mRobots.clear();

	mRobotControlCount = 0;
	mRobotControlMax = 0;
}


void RobotPool::erase(Robot* robot)
{
	mRobots.erase(find(mRobots.begin(), mRobots.end(), robot));

	eraseRobot(mDiggers, robot);
	eraseRobot(mDozers, robot);
	eraseRobot(mMiners, robot);
}


/**
 * Adds a robot of specified type to the pool.
 *
 * \return Returns a pointer to the robot, or nullptr if type was invalid.
 */
Robot* RobotPool::addRobot(Robot::Type type)
{
	switch (type)
	{
	case Robot::Type::Dozer:
		mDozers.push_back(new Robodozer());
		mRobots.push_back(mDozers.back());
		return mDozers.back();

	case Robot::Type::Digger:
		mDiggers.push_back(new Robodigger());
		mRobots.push_back(mDiggers.back());
		return mDiggers.back();

	case Robot::Type::Miner:
		mMiners.push_back(new Robominer());
		mRobots.push_back(mMiners.back());
		return mMiners.back();

	default:
		throw std::runtime_error("Unknown Robot::Type: " + std::to_string(static_cast<int>(type)));
	}

	return nullptr;
}


/**
 * Gets an idle Robodigger from the pool.
 */
Robodigger& RobotPool::getDigger()
{
	return getIdleRobot(mDiggers);
}


/**
 * Gets an idle Robodozer from the pool.
 */
Robodozer& RobotPool::getDozer()
{
	return getIdleRobot(mDozers);
}


/**
 * Gets an idle Robominer from the pool.
 */
Robominer& RobotPool::getMiner()
{
	return getIdleRobot(mMiners);
}


/**
 * Determines if a requested robot type is available.
 * 
 * \return	Returns true if the requested robot type is available. False otherwise.
 */
bool RobotPool::robotAvailable(Robot::Type type) const
{
	switch (type)
	{
	case Robot::Type::Digger:
	{
		return getIdleRobotOrNull(mDiggers) != nullptr;
	}
	case Robot::Type::Dozer:
	{
		return getIdleRobotOrNull(mDozers) != nullptr;
	}
	case Robot::Type::Miner:
	{
		return getIdleRobotOrNull(mMiners) != nullptr;
	}
	default:
	{
		return false;
	}
	}
}


std::size_t RobotPool::getAvailableCount(Robot::Type type) const
{
	switch (type)
	{
	case Robot::Type::Digger:
		return getIdleCount(mDiggers);

	case Robot::Type::Dozer:
		return getIdleCount(mDozers);

	case Robot::Type::Miner:
		return getIdleCount(mMiners);

	default:
		return 0;
	}
}


void RobotPool::InitRobotCtrl(std::size_t maxRobotCtrl)
{
	mRobotControlMax = maxRobotCtrl;
	mRobotControlCount = robotControlCount(mDiggers) + robotControlCount(mDozers) + robotControlCount(mMiners);
}


void RobotPool::AddRobotCtrl()
{
	if (mRobotControlCount < mRobotControlMax)
	{
		++mRobotControlCount;
	}
}


bool RobotPool::insertRobotIntoTable(RobotTileTable& robotMap, Robot* robot, Tile* tile)
{
	if (!tile) { return false; }

	auto it = robotMap.find(robot);
	if (it != robotMap.end()) { throw std::runtime_error("MapViewState::insertRobot(): Attempting to add a duplicate Robot* pointer."); }

	robotMap[robot] = tile;
	tile->pushThing(robot);

	AddRobotCtrl();

	return true;
}

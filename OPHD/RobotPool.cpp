#include "RobotPool.h"
#include "RobotPoolHelper.h"
#include "Map/Tile.h"

#include <algorithm>


int ROBOT_ID_COUNTER = 0; /// \fixme Kludge


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
 * Generates a new unique ID for the robot.
 *
 * \return Returns a pointer to the robot, or nullptr if type was invalid.
 */
Robot* RobotPool::addRobot(Robot::Type type)
{
	// Generate a new unique ID
	const auto id = ++ROBOT_ID_COUNTER;
	return addRobot(type, id);
}


/**
 * Adds a robot of specified type to the pool.
 *
 * \return Returns a pointer to the robot, or nullptr if type was invalid.
 */
Robot* RobotPool::addRobot(Robot::Type type, int id)
{
	switch (type)
	{
	case Robot::Type::Dozer:
		mDozers.push_back(new Robodozer());
		mDozers.back()->id(id);
		mRobots.push_back(mDozers.back());
		return mDozers.back();

	case Robot::Type::Digger:
		mDiggers.push_back(new Robodigger());
		mDiggers.back()->id(id);
		mRobots.push_back(mDiggers.back());
		return mDiggers.back();

	case Robot::Type::Miner:
		mMiners.push_back(new Robominer());
		mMiners.back()->id(id);
		mRobots.push_back(mMiners.back());
		return mMiners.back();

	default:
		break;
	}

	return nullptr;
}


/**
 * Gets a Robodigger from the pool.
 * 
 * \return	Returns a pointer to an available Robodigger. If no digger is available, returns nullptr.
 */
Robodigger* RobotPool::getDigger()
{
	return static_cast<Robodigger*>(getIdleRobot(mDiggers));
}


/**
 * Gets a Robodozer from the pool.
 *
 * \return	Returns a pointer to an available Robodozer. If no dozer is available, returns nullptr.
 */
Robodozer* RobotPool::getDozer()
{
	return static_cast<Robodozer*>(getIdleRobot(mDozers));
}


/**
 * Gets a Robominer from the pool.
 *
 * \return	Returns a pointer to an available Robominer. If no miner is available, returns nullptr.
 */
Robominer* RobotPool::getMiner()
{
	return static_cast<Robominer*>(getIdleRobot(mMiners));
}


/**
 * Determines if a requested robot type is available.
 * 
 * \return	Returns true if the requested robot type is available. False otherwise.
 */
bool RobotPool::robotAvailable(Robot::Type type)
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


int RobotPool::getAvailableCount(Robot::Type type)
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


void RobotPool::InitRobotCtrl(uint32_t maxRobotCtrl)
{
	mRobotControlMax = maxRobotCtrl;
	mRobotControlCount = 0;

	updateRobotControlCount(mDiggers, mRobotControlCount);
	updateRobotControlCount(mDozers, mRobotControlCount);
	updateRobotControlCount(mMiners, mRobotControlCount);
}


void RobotPool::AddRobotCtrl()
{
	if (mRobotControlMax > mRobotControlCount)
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

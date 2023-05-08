#include "RobotPool.h"

#include "StructureManager.h"
#include "Map/Tile.h"
#include "MapObjects/Structures/CommandCenter.h"
#include "MapObjects/Structures/RobotCommand.h"

#include <NAS2D/Utility.h>

#include <algorithm>
#include <stdexcept>


namespace
{
	template <class T>
	void eraseRobot(T& list, Robot* robot)
	{
		for (auto it = list.begin(); it != list.end(); ++it)
		{
			if (&*it == robot)
			{
				list.erase(it);
				return;
			}
		}
	}


	template <class T>
	bool hasIdleRobot(const T& list)
	{
		for (auto& robot : list)
		{
			if (robot.idle()) { return true; }
		}
		return false;
	}


	template <class T>
	auto& getIdleRobot(T& list)
	{
		for (auto& robot : list)
		{
			if (robot.idle()) { return robot; }
		}
		throw std::runtime_error("Failed to get an idle robot");
	}


	template <class T>
	std::size_t getIdleCount(const T& list)
	{
		std::size_t count = 0;
		for (const auto& robot : list)
		{
			if (robot.idle()) { ++count; }
		}

		return count;
	}


	template <class T>
	std::size_t robotControlCount(const T& list)
	{
		std::size_t controlCounter{0};
		for (const auto& robot : list)
		{
			if (!robot.idle() && !robot.isDead()) { ++controlCounter; }
		}
		return controlCounter;
	}
}


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
	mDiggers.clear();
	mDozers.clear();
	mMiners.clear();
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
 * \return Returns a reference to the robot, or throws if type was invalid.
 */
Robot& RobotPool::addRobot(Robot::Type type)
{
	switch (type)
	{
	case Robot::Type::Dozer:
		mDozers.emplace_back();
		mRobots.push_back(&mDozers.back());
		break;
	case Robot::Type::Digger:
		mDiggers.emplace_back();
		mRobots.push_back(&mDiggers.back());
		break;
	case Robot::Type::Miner:
		mMiners.emplace_back();
		mRobots.push_back(&mMiners.back());
		break;
	default:
		throw std::runtime_error("Unknown Robot::Type: " + std::to_string(static_cast<int>(type)));
	}

	return *mRobots.back();
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
		return hasIdleRobot(mDiggers);
	}
	case Robot::Type::Dozer:
	{
		return hasIdleRobot(mDozers);
	}
	case Robot::Type::Miner:
	{
		return hasIdleRobot(mMiners);
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


void RobotPool::update()
{
	const auto& commandCenters = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();
	const auto& robotCommands = NAS2D::Utility<StructureManager>::get().getStructures<RobotCommand>();

	// 3 for the first command center
	std::size_t maxRobots = 0;
	if (commandCenters.size() > 0) { maxRobots += 3; }
	// the 10 per robot command facility
	for (std::size_t s = 0; s < robotCommands.size(); ++s)
	{
		if (robotCommands[s]->operational()) { maxRobots += 10; }
	}

	mRobotControlMax = maxRobots;
	mRobotControlCount = robotControlCount(mDiggers) + robotControlCount(mDozers) + robotControlCount(mMiners);
}


void RobotPool::insertRobotIntoTable(RobotTileTable& robotMap, Robot& robot, Tile& tile)
{
	// Add pre-check for control count against max capacity, with one caveat
	// When loading saved games a control max won't have been set yet as robots are loaded before structures
	// Assume saved games are correct, and if not, things will be corrected by next turn
	if (mRobotControlMax > 0 && mRobotControlCount >= mRobotControlMax)
	{
		throw std::runtime_error("Must increase robot command capacity before placing more robots: " + std::to_string(mRobotControlCount) + "/" + std::to_string(mRobotControlMax));
	}

	auto it = robotMap.find(&robot);
	if (it != robotMap.end()) { throw std::runtime_error("MapViewState::insertRobot(): Attempting to add a duplicate Robot* pointer."); }

	robotMap[&robot] = &tile;
	tile.pushMapObject(&robot);

	++mRobotControlCount;
}

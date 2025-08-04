#include "RobotPool.h"

#include "StructureManager.h"
#include "Map/Tile.h"
#include "MapObjects/Robots.h"
#include "MapObjects/Structures/CommandCenter.h"

#include <libOPHD/MapObjects/StructureType.h>

#include <NAS2D/Utility.h>
#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>

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


RobotPool::RobotPool() :
	mStructureManager{NAS2D::Utility<StructureManager>::get()}
{
}


/**
 * Frees all resources from the robot pool so this class should be
 * freed after all pointers to robots attained from the pool have been
 * released.
 */
RobotPool::~RobotPool()
{
	clear();
}


RobotPool::DiggerList& RobotPool::diggers() { return mDiggers; }
RobotPool::DozerList& RobotPool::dozers() { return mDozers; }
RobotPool::MinerList& RobotPool::miners() { return mMiners; }

const RobotPool::DiggerList& RobotPool::diggers() const { return mDiggers; }
const RobotPool::DozerList& RobotPool::dozers() const { return mDozers; }
const RobotPool::MinerList& RobotPool::miners() const { return mMiners; }


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
 * Adds a robot of specified robotTypeIndex to the pool.
 *
 * \return Returns a reference to the robot, or throws if robotTypeIndex was invalid.
 */
Robot& RobotPool::addRobot(RobotTypeIndex robotTypeIndex)
{
	switch (robotTypeIndex)
	{
	case RobotTypeIndex::Dozer:
		mDozers.emplace_back();
		mRobots.push_back(&mDozers.back());
		break;
	case RobotTypeIndex::Digger:
		mDiggers.emplace_back();
		mRobots.push_back(&mDiggers.back());
		break;
	case RobotTypeIndex::Miner:
		mMiners.emplace_back();
		mRobots.push_back(&mMiners.back());
		break;
	default:
		throw std::runtime_error("Unknown RobotTypeIndex: " + std::to_string(static_cast<int>(robotTypeIndex)));
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
 * Determines if a requested robot robotTypeIndex is available.
 *
 * \return	Returns true if the requested robot robotTypeIndex is available. False otherwise.
 */
bool RobotPool::robotAvailable(RobotTypeIndex robotTypeIndex) const
{
	switch (robotTypeIndex)
	{
	case RobotTypeIndex::Digger:
	{
		return hasIdleRobot(mDiggers);
	}
	case RobotTypeIndex::Dozer:
	{
		return hasIdleRobot(mDozers);
	}
	case RobotTypeIndex::Miner:
	{
		return hasIdleRobot(mMiners);
	}
	default:
	{
		return false;
	}
	}
}


std::size_t RobotPool::getAvailableCount(RobotTypeIndex robotTypeIndex) const
{
	switch (robotTypeIndex)
	{
	case RobotTypeIndex::Digger:
		return getIdleCount(mDiggers);

	case RobotTypeIndex::Dozer:
		return getIdleCount(mDozers);

	case RobotTypeIndex::Miner:
		return getIdleCount(mMiners);

	default:
		return 0;
	}
}


void RobotPool::update()
{
	const auto& structures = mStructureManager.allStructures();

	int totalRobotCommandCapacity = 0;
	for (const auto* structure : structures)
	{
		if (structure->operational()) { totalRobotCommandCapacity += structure->type().robotCommandCapacity; }
	}

	// Special case hack to allow robot use during initial colony deploy
	if (totalRobotCommandCapacity == 0)
	{
		const auto& commandCenters = mStructureManager.getStructures<CommandCenter>();
		if (commandCenters.size() > 0)
		{
			totalRobotCommandCapacity += commandCenters[0]->type().robotCommandCapacity;
		}
	}

	mRobotControlMax = static_cast<std::size_t>(totalRobotCommandCapacity);
	mRobotControlCount = robotControlCount(mDiggers) + robotControlCount(mDozers) + robotControlCount(mMiners);
}


void RobotPool::insertRobotIntoTable(std::vector<Robot*>& deployedRobots, Robot& robot, Tile& tile)
{
	// Add pre-check for control count against max capacity, with one caveat
	// When loading saved games a control max won't have been set yet as robots are loaded before structures
	// Assume saved games are correct, and if not, things will be corrected by next turn
	if (mRobotControlMax > 0 && mRobotControlCount >= mRobotControlMax)
	{
		throw std::runtime_error("Must increase robot command capacity before placing more robots: " + std::to_string(mRobotControlCount) + " / " + std::to_string(mRobotControlMax));
	}

	auto it = std::find(deployedRobots.begin(), deployedRobots.end(), &robot);
	if (it != deployedRobots.end()) { throw std::runtime_error("MapViewState::insertRobot(): Attempting to add a duplicate Robot* pointer."); }

	deployedRobots.push_back(&robot);
	tile.mapObject(&robot);

	++mRobotControlCount;
}


NAS2D::Xml::XmlElement* RobotPool::writeRobots()
{
	auto* robots = new NAS2D::Xml::XmlElement("robots");

	for (auto robot : mRobots)
	{
		robots->linkEndChild(NAS2D::dictionaryToAttributes("robot", robot->getDataDict()));
	}

	return robots;
}

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "RobotPool.h"


/**
 * C'tor
 */
RobotPool::RobotPool()
{}


/**
 * D'tor.
 * 
 * Frees all resources from the robot pool so this class should be
 * freed after all pointers to robots attained from the pool have been
 * released.
 */
RobotPool::~RobotPool()
{
	clear();
}


/**
 * 
 */
void RobotPool::clear()
{
	for (size_t i = 0; i < mDiggers.size(); i++)
	{
		delete mDiggers[i];
	}

	for (size_t i = 0; i < mDozers.size(); i++)
	{
		delete mDozers[i];
	}

	for (size_t i = 0; i < mMiners.size(); i++)
	{
		delete mMiners[i];
	}

	mDiggers.clear();
	mDozers.clear();
	mMiners.clear();
	mRobotControlCount = 0;
	mRobotControlMax = 0;
}


/**
 * Adds a robot of specified type to the pool.
 *
 * \return Returns a pointer to the recently.
 * \return Returns a nullptr if the robot type was invalid or unsupported.
 */
Robot* RobotPool::addRobot(RobotType _type)
{
	switch (_type)
	{
	case ROBOT_DOZER:
		mDozers.push_back(new Robodozer());
		return mDozers.back();
		break;

	case ROBOT_DIGGER:
		mDiggers.push_back(new Robodigger());
		return mDiggers.back();
		break;

	case ROBOT_MINER:
		mMiners.push_back(new Robominer());
		return mMiners.back();
		break;

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
	for (size_t i = 0; i < mDiggers.size(); i++)
	{
		if (mDiggers[i]->idle()) { return mDiggers[i]; }
	}

	return nullptr;
}


/**
 * Gets a Robodozer from the pool.
 *
 * \return	Returns a pointer to an available Robodozer. If no dozer is available, returns nullptr.
 */
Robodozer* RobotPool::getDozer()
{
	for (size_t i = 0; i < mDozers.size(); i++)
	{
		if (mDozers[i]->idle()) { return mDozers[i]; }
	}

	return nullptr;
}


/**
 * Gets a Robominer from the pool.
 *
 * \return	Returns a pointer to an available Robominer. If no miner is available, returns nullptr.
 */
Robominer* RobotPool::getMiner()
{
	for (size_t i = 0; i < mMiners.size(); i++)
	{
		if (mMiners[i]->idle()) { return mMiners[i]; }
	}

	return nullptr;
}


/**
 * Determines if a requested robot type is available.
 * 
 * \return	Returns true if the requested robot type is available. False otherwise.
 */
bool RobotPool::robotAvailable(RobotType _type)
{
	switch (_type)
	{
	case ROBOT_DIGGER:
		return getDigger() != nullptr;
		break;

	case ROBOT_DOZER:
		return getDozer() != nullptr;
		break;

	case ROBOT_MINER:
		return getMiner() != nullptr;
		break;

	default:
		return false;
		break;
	}

	return false;
}


/**
 * Determines if all robotos of all types are busy.
 * 
 * \todo	Is this function really necessary?
 */
bool RobotPool::allRobotsBusy()
{
	for (size_t i = 0; i < mDozers.size(); i++)
	{
		if (mDozers[i]->idle()) { return false; }
	}

	for (size_t i = 0; i < mDiggers.size(); i++)
	{
		if (mDiggers[i]->idle()) { return false; }
	}

	for (size_t i = 0; i < mMiners.size(); i++)
	{
		if (mMiners[i]->idle()) { return false; }
	}

	return true;
}


/**
 * 
 */
int RobotPool::getAvailableCount(RobotType _type)
{
	int count = 0;
	switch (_type)
	{
	case ROBOT_DIGGER:
		for (size_t i = 0; i < mDiggers.size(); i++)
		{
			if (mDiggers[i]->idle()) count++;
		}
		break;

	case ROBOT_DOZER:
		for (size_t i = 0; i < mDozers.size(); i++)
		{
			if (mDozers[i]->idle()) count++;
		}
		break;

	case ROBOT_MINER:
		for (size_t i = 0; i < mMiners.size(); i++)
		{
			if (mMiners[i]->idle()) count++;
		}
		break;

	default:
		return 0;
		break;
	}

	return count;
}


/**
 * 
 */
void RobotPool::InitRobotCtrl(uint32_t maxRobotCtrl)
{
	mRobotControlMax = maxRobotCtrl;
	mRobotControlCount = 0;

	for (size_t i = 0; i < mDiggers.size(); ++i)
	{
		if (!mDiggers[i]->idle() && !mDiggers[i]->dead()) { mRobotControlCount++; }
	}

	for (size_t i = 0; i < mDozers.size(); ++i)
	{
		if (!mDozers[i]->idle() && !mDozers[i]->dead()) { mRobotControlCount++; }
	}

	for (size_t i = 0; i < mMiners.size(); ++i)
	{
		if (!mMiners[i]->idle() && !mMiners[i]->dead()) { mRobotControlCount++; }
	}
}


/**
 * 
 */
void RobotPool::AddRobotCtrl()
{
	if (mRobotControlMax > mRobotControlCount)
	{
		mRobotControlCount++;
	}
}


/**
 * 
 */
bool RobotPool::insertRobotIntoTable(RobotTileTable& _rm, Robot* _r, Tile* _t)
{
	if (!_t) { return false; }

	auto it = _rm.find(_r);
	if (it != _rm.end()) { throw std::runtime_error("GameState::insertRobot(): Attempting to add a duplicate Robot* pointer."); }

	_rm[_r] = _t;
	_t->pushThing(_r);

	AddRobotCtrl();

	return true;
}

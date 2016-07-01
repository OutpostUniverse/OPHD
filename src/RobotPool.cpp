#include "RobotPool.h"


/**
 * C'tor
 */
RobotPool::RobotPool()
{
}


/**
 * D'tor.
 * 
 * Frees all resources from the robot pool so this class should be
 * freed after all pointers to robots attained from the pool have been
 * released.
 */
RobotPool::~RobotPool()
{
	for (size_t i = 0; i < mDozers.size(); i++)
		delete mDozers[i];

	for (size_t i = 0; i < mDiggers.size(); i++)
		delete mDiggers[i];

	for (size_t i = 0; i < mMiners.size(); i++)
		delete mMiners[i];
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
		if (mDiggers[i]->idle())
			return mDiggers[i];
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
		if (mDozers[i]->idle())
			return mDozers[i];
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
		if (mMiners[i]->idle())
			return mMiners[i];
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
	switch(_type)
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
		if (mDozers[i]->idle())
			return false;

	for (size_t i = 0; i < mDiggers.size(); i++)
		if (mDiggers[i]->idle())
			return false;

	for (size_t i = 0; i < mMiners.size(); i++)
		if (mMiners[i]->idle())
			return false;

	return true;
}


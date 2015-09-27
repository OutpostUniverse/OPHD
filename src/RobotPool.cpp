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
 * Gets a robot from the pool of the specified type.
 * 
 * \return	Returns a nullptr if the requested robot type is not available.
 */
Robot* RobotPool::getRobot(RobotType _type)
{
	switch (_type)
	{
	case ROBO_DOZER:
		for (size_t i = 0; i < mDozers.size(); i++)
		{
			if (mDozers[i]->idle())
				return mDozers[i];
		}
		break;
	case ROBO_DIGGER:
		for (size_t i = 0; i < mDiggers.size(); i++)
		{
			if (mDiggers[i]->idle())
				return mDiggers[i];
		}
		break;
	case ROBO_MINER:
		for (size_t i = 0; i < mMiners.size(); i++)
		{
			if (mMiners[i]->idle())
				return mMiners[i];
		}
		break;
	}

	return nullptr;
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
	case ROBO_DOZER:
		mDozers.push_back(new Robodozer());
		return mDozers.back();
		break;
	case ROBO_DIGGER:
		mDiggers.push_back(new Robodigger());
		return mDiggers.back();
		break;
	case ROBO_MINER:
		mMiners.push_back(new Robominer());
		return mMiners.back();
		break;
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
	Robot* r = getRobot(_type);
	return r != nullptr;
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


void RobotPool::dozerSelfDestruct()
{
}

void RobotPool::diggerSelfDestruct()
{
}

void RobotPool::minerSelfDestruct()
{
}

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "RobotPool.h"
#include "RobotPoolHelper.h"

extern int ROBOT_ID_COUNTER; /// \fixme	Kludge


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
	clearRobots(mDiggers);
	clearRobots(mDozers);
	clearRobots(mMiners);

	mRobotControlCount = 0;
	mRobotControlMax = 0;
}


/**
 * Adds a robot of specified type to the pool.
 *
 * \return Returns a pointer to the recently.
 * \return Returns a nullptr if the robot type was invalid or unsupported.
 */
Robot* RobotPool::addRobot(RobotType _type, int id)
{
	int _id = 0;
	if (id == 0) { _id = ++ROBOT_ID_COUNTER; }
	else { _id = id; }

	switch (_type)
	{
	case ROBOT_DOZER:
		mDozers.push_back(new Robodozer());
		mDozers.back()->id(_id);
		return mDozers.back();
		break;

	case ROBOT_DIGGER:
		mDiggers.push_back(new Robodigger());
		mDiggers.back()->id(_id);
		return mDiggers.back();
		break;

	case ROBOT_MINER:
		mMiners.push_back(new Robominer());
		mMiners.back()->id(_id);
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
	return static_cast<Robodigger*>(getRobot(mDiggers));
}


/**
 * Gets a Robodozer from the pool.
 *
 * \return	Returns a pointer to an available Robodozer. If no dozer is available, returns nullptr.
 */
Robodozer* RobotPool::getDozer()
{
	return static_cast<Robodozer*>(getRobot(mDozers));
}


/**
 * Gets a Robominer from the pool.
 *
 * \return	Returns a pointer to an available Robominer. If no miner is available, returns nullptr.
 */
Robominer* RobotPool::getMiner()
{
	return static_cast<Robominer*>(getRobot(mMiners));
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
 * 
 */
int RobotPool::getAvailableCount(RobotType _type)
{
	switch (_type)
	{
	case ROBOT_DIGGER:
		return getIdleCount(mDiggers);

	case ROBOT_DOZER:
		return getIdleCount(mDozers);

	case ROBOT_MINER:
		return getIdleCount(mMiners);

	default:
		return 0;
	}
}


/**
 * 
 */
void RobotPool::InitRobotCtrl(uint32_t maxRobotCtrl)
{
	mRobotControlMax = maxRobotCtrl;
	mRobotControlCount = 0;

	updateRobotControlCount(mDiggers, mRobotControlCount);
	updateRobotControlCount(mDozers, mRobotControlCount);
	updateRobotControlCount(mMiners, mRobotControlCount);
}


/**
 * 
 */
void RobotPool::AddRobotCtrl()
{
	if (mRobotControlMax > mRobotControlCount)
	{
		++mRobotControlCount;
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

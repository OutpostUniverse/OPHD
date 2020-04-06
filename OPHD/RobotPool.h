#pragma once


#include "Things/Robots/Robots.h"
#include "Map/Tile.h"


class RobotPool
{
public:
	typedef std::vector<Robodigger*> DiggerList;
	typedef std::vector<Robodozer*> DozerList;
	typedef std::vector<Robominer*> MinerList;
	typedef std::map<Robot*, Tile*> RobotTileTable;

public:
	RobotPool();
	~RobotPool();

	Robot* addRobot(RobotType type, int id = 0);

	Robodigger* getDigger();
	Robodozer* getDozer();
	Robominer* getMiner();

	bool robotAvailable(RobotType type);
	int getAvailableCount(RobotType type);

	void InitRobotCtrl(uint32_t MaxRobotCtrl);
	bool robotCtrlAvailable() { return mRobotControlCount < mRobotControlMax; }
	bool commandCapacityAvailable() { return mRobots.size() < mRobotControlMax; }
	void AddRobotCtrl();

	DiggerList& diggers() { return mDiggers; }
	DozerList& dozers() { return mDozers; }
	MinerList& miners() { return mMiners; }

	void clear();
	void erase(Robot* r);
	bool insertRobotIntoTable(RobotTileTable& robotMap, Robot* robot, Tile* tile);

	uint32_t robotControlMax() { return mRobotControlMax; }
	uint32_t currentControlCount() { return mRobotControlCount; }
	uint32_t availableControlCount() { return robotControlMax() - currentControlCount(); }

	const RobotList& robots() const { return mRobots; }

private:
	DiggerList		mDiggers;
	DozerList		mDozers;
	MinerList		mMiners;

	RobotList		mRobots;	// List of all robots by pointer to base class

	uint32_t		mRobotControlMax = 0;
	uint32_t		mRobotControlCount = 0;
};

#pragma once


#include "Things/Robots/Robots.h"


class Tile;


class RobotPool
{
public:
	using DiggerList = std::vector<Robodigger*>;
	using DozerList = std::vector<Robodozer*>;
	using MinerList = std::vector<Robominer*>;
	using RobotTileTable = std::map<Robot*, Tile*>;

public:
	RobotPool();
	~RobotPool();

	Robot* addRobot(Robot::Type type);
	Robot* addRobot(Robot::Type type, int id);

	Robodigger& getDigger();
	Robodozer& getDozer();
	Robominer& getMiner();

	bool robotAvailable(Robot::Type type);
	int getAvailableCount(Robot::Type type);

	void InitRobotCtrl(uint32_t MaxRobotCtrl);
	bool robotCtrlAvailable() { return mRobotControlCount < mRobotControlMax; }
	bool commandCapacityAvailable() { return mRobots.size() < mRobotControlMax; }
	void AddRobotCtrl();

	DiggerList& diggers() { return mDiggers; }
	DozerList& dozers() { return mDozers; }
	MinerList& miners() { return mMiners; }

	void clear();
	void erase(Robot* robot);
	bool insertRobotIntoTable(RobotTileTable& robotMap, Robot* robot, Tile* tile);

	uint32_t robotControlMax() { return mRobotControlMax; }
	uint32_t currentControlCount() { return mRobotControlCount; }
	uint32_t availableControlCount() { return robotControlMax() - currentControlCount(); }

	const RobotList& robots() const { return mRobots; }

private:
	DiggerList mDiggers;
	DozerList mDozers;
	MinerList mMiners;

	RobotList mRobots; // List of all robots by pointer to base class

	uint32_t mRobotControlMax = 0;
	uint32_t mRobotControlCount = 0;
};

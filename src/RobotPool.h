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

	Robot* addRobot(RobotType _type, int id = 0);

	Robodigger* getDigger();
	Robodozer* getDozer();
	Robominer* getMiner();

	bool robotAvailable(RobotType _type);
	bool allRobotsBusy();
	int getAvailableCount(RobotType _type);

	void InitRobotCtrl(uint32_t MaxRobotCtrl);
	bool robotCtrlAvailable() { return mRobotControlCount < mRobotControlMax; };
	void AddRobotCtrl();

	DiggerList& diggers() { return mDiggers; }
	DozerList& dozers() { return mDozers; }
	MinerList& miners() { return mMiners; }

	void clear();
	bool insertRobotIntoTable(RobotTileTable& _rm, Robot* _r, Tile* _t);

	uint32_t robotControlMax() { return mRobotControlMax; };
	uint32_t currentControlCount() { return mRobotControlCount; };

private:
	DiggerList		mDiggers;
	DozerList		mDozers;
	MinerList		mMiners;

	uint32_t		mRobotControlMax = 0;
	uint32_t		mRobotControlCount = 0;
};

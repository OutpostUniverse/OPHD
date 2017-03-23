#pragma once

#include "Things/Robots/Robots.h"

class RobotPool
{
public:
	typedef std::vector<Robodigger*> DiggerList;
	typedef std::vector<Robodozer*> DozerList;
	typedef std::vector<Robominer*> MinerList;

public:

	RobotPool();
	~RobotPool();

	Robot* addRobot(RobotType _type);

	Robodigger* getDigger();
	Robodozer* getDozer();
	Robominer* getMiner();

	bool robotAvailable(RobotType _type);
	bool allRobotsBusy();
	int getAvailableCount(RobotType _type);

	DiggerList& diggers() { return mDiggers; }
	DozerList& dozers() { return mDozers; }
	MinerList& miners() { return mMiners; }

	void clear();

protected:
private:

	DiggerList		mDiggers;
	DozerList		mDozers;
	MinerList		mMiners;
};

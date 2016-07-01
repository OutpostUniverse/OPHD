#pragma once

#include "Things/Robots/Robots.h"

class RobotPool
{
public:
	typedef vector<Robodigger*> DiggerList;
	typedef vector<Robodozer*> DozerList;
	typedef vector<Robominer*> MinerList;

public:

	RobotPool();
	~RobotPool();

	Robot* addRobot(RobotType _type);

	Robodigger* getDigger();
	Robodozer* getDozer();
	Robominer* getMiner();

	bool robotAvailable(RobotType _type);
	bool allRobotsBusy();

	DiggerList& diggers() { return mDiggers; }
	DozerList& dozers() { return mDozers; }
	MinerList& miners() { return mMiners; }

protected:
private:

	DiggerList		mDiggers;
	DozerList		mDozers;
	MinerList		mMiners;
};

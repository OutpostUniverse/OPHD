#pragma once

#include "Things/Robots/Robots.h"

class RobotPool
{
public:

	RobotPool();
	~RobotPool();

	Robot* addRobot(RobotType _type);

	Robodigger* getDigger();
	Robodozer* getDozer();
	Robominer* getMiner();

	bool robotAvailable(RobotType _type);
	bool allRobotsBusy();

protected:
private:

	vector<Robodozer*>		mDozers;
	vector<Robodigger*>		mDiggers;
	vector<Robominer*>		mMiners;
};

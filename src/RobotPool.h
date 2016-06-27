#pragma once

#include "Things/Robots/Robots.h"

class RobotPool
{
public:

	enum RobotType
	{
		ROBO_DOZER,
		ROBO_DIGGER,
		ROBO_MINER
	};

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

	void dozerSelfDestruct();
	void diggerSelfDestruct();
	void minerSelfDestruct();

	vector<Robodozer*>		mDozers;
	vector<Robodigger*>		mDiggers;
	vector<Robominer*>		mMiners;
};

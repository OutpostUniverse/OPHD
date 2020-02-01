#pragma once

#include "Structure.h"

#include "../Robots/Robot.h"

#include "../../Constants.h"

#include <vector>


/**
 * Implements the Robot Command structure.
 */
class RobotCommand : public Structure
{
public:
	RobotCommand() : Structure(constants::ROBOT_COMMAND, "structures/robot_control.sprite", CLASS_ROBOT_COMMAND)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(false);
	}

	virtual ~RobotCommand()	{}

	bool commandedByThis(Robot* _r) const;

	bool commandCapacityAvailable() const;
	void addRobot(Robot* _r);
	void removeRobot(Robot* _r);

	const RobotList& robots() { return mRobotList; }

protected:
	virtual void think() final {}
	virtual void defineResourceOutput() final {}

	virtual void defineResourceInput() final
	{
		resourcesIn().energy(5);
	}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:
	RobotList	mRobotList;
};

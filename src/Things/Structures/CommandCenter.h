#pragma once

#include "Structure.h"

/**
 * Implements the Command Center structure.
 */
class CommandCenter: public Structure
{
public:
	CommandCenter(): Structure(constants::COMMAND_CENTER, "structures/command_center.sprite", CLASS_COMMAND)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
		selfSustained(true);
	}

	virtual ~CommandCenter()
	{}

protected:
	virtual void defineResourceInput() {}
	virtual void defineResourceOutput() {}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:
};

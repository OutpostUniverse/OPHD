#pragma once

#include "Structure.h"

class CommandCenter: public Structure
{
public:
	CommandCenter():	Structure(constants::COMMAND_CENTER, "structures/command_center.sprite", STRUCTURE_COMMAND)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(false);
		selfSustained(true);
	}

	virtual ~CommandCenter()
	{}

protected:
	
	virtual void defineResourceInput()
	{
	}

	virtual void defineResourceOutput()
	{
	}

private:

};

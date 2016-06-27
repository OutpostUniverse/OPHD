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
		defineResourceCostToBuild();
	}

	~CommandCenter()
	{}

protected:
	
	virtual void defineResourceInput()
	{
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(100);
		resourcesCostToBuild().rareMetals(65);
		resourcesCostToBuild().commonMinerals(75);
		resourcesCostToBuild().rareMinerals(35);
	}

private:

};

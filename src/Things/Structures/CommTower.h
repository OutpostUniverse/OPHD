#pragma once

#include "Structure.h"

#include "../../Constants.h"

class CommTower :	public Structure
{
public:

	CommTower() : Structure(constants::COMM_TOWER, "structures/communications_tower.sprite", CLASS_COMM)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}

	virtual ~CommTower()
	{}

protected:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(3);
	}

	virtual void defineResourceOutput()
	{}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

};


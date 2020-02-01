#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Park : public Structure
{
public:
	Park() : Structure(constants::PARK, "structures/park.sprite", CLASS_PARK)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}


	virtual ~Park()
	{}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:

};

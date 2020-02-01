#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Commercial : public Structure
{
public:
	Commercial() : Structure(constants::COMMERCIAL, "structures/commercial.sprite", CLASS_COMMERCIAL)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}


	virtual ~Commercial()
	{}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(2);
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

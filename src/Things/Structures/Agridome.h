#pragma once

#include "Structure.h"

const int AGRIDOME_CAPACITY = 1000;
const int AGRIDOME_BASE_PRODUCUCTION = 10;

class Agridome : public Structure
{
public:
	Agridome() : Structure(constants::AGRIDOME, "structures/agridome.sprite", CLASS_FOOD_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(3);

		requiresCHAP(true);
	}

	virtual ~Agridome()
	{}

protected:

	virtual void think()
	{
		if (isIdle())
			return;

		if (storage().food() == AGRIDOME_CAPACITY)
		{
			idle();
		}
		else
		{
			int curr_food = storage().food();
			if (curr_food > AGRIDOME_CAPACITY - AGRIDOME_BASE_PRODUCUCTION)
			{
				storage().food(AGRIDOME_CAPACITY);
				idle();
			}
			else
			{
				storage().food(curr_food + AGRIDOME_BASE_PRODUCUCTION);
			}
		}

	}

	virtual void defineResourceInput()
	{
		resourcesIn().commonMinerals(1);
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

	virtual void disabledStateSet()
	{
		// Clear food store when disabled.
		storage().food(0);
	}

private:

};

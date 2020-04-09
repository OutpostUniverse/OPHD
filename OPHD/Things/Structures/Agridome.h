#pragma once

#include "Structure.h"

const int AGRIDOME_CAPACITY = 1000;
const int AGRIDOME_BASE_PRODUCUCTION = 10;

class Agridome : public Structure
{
public:
	Agridome() : Structure(constants::AGRIDOME, "structures/agridome.sprite", StructureClass::CLASS_FOOD_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(3);

		requiresCHAP(true);
	}

protected:
	void think() override
	{
		if (isIdle())
			return;

		if (storage().food() == AGRIDOME_CAPACITY)
		{
			idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
		}
		else
		{
			int curr_food = storage().food();
			if (curr_food > AGRIDOME_CAPACITY - AGRIDOME_BASE_PRODUCUCTION)
			{
				storage().food(AGRIDOME_CAPACITY);
				idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
			}
			else
			{
				storage().food(curr_food + AGRIDOME_BASE_PRODUCUCTION);
			}
		}

	}

	void defineResourceInput() override
	{
		resourcesIn().commonMinerals(1);
		resourcesIn().energy(2);
	}

	void disabledStateSet() override
	{
		// Clear food store when disabled.
		storage().food(0);
	}
};

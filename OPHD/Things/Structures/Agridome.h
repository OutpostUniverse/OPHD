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

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 2);

		stringTable[{0, 0}].text = "Food Stored:";
		stringTable[{1, 0}].text = std::to_string(storage().food()) + " / " + std::to_string(AGRIDOME_CAPACITY);

		stringTable[{0, 1}].text = "Production Rate:";
		stringTable[{1, 1}].text = std::to_string(calculateProduction());

		return stringTable;
	}

protected:
	void think() override
	{
		if (isIdle())
			return;

		storage().food(storage().food() + calculateProduction());

		if (isStorageFull())
		{
			idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
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

private:
	int calculateProduction()
	{
		if (!operational())
		{
			return 0;
		}

		if (AGRIDOME_CAPACITY < storage().food() + AGRIDOME_BASE_PRODUCUCTION)
		{
			return AGRIDOME_CAPACITY - storage().food();
		}

		return AGRIDOME_BASE_PRODUCUCTION;
	}

	bool isStorageFull()
	{
		return storage().food() == AGRIDOME_CAPACITY;
	}
};

#pragma once

#include "Structure.h"
#include <algorithm>

const int AGRIDOME_CAPACITY = 1000;
const int AGRIDOME_BASE_PRODUCUCTION = 10;

class Agridome : public Structure
{
public:
	Agridome() : Structure(constants::AGRIDOME, "structures/agridome.sprite", StructureClass::FoodProduction)
	{
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
			idle(IdleReason::InternalStorageFull);
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

		return std::min(AGRIDOME_BASE_PRODUCUCTION, AGRIDOME_CAPACITY - storage().food());
	}

	bool isStorageFull()
	{
		return storage().food() == AGRIDOME_CAPACITY;
	}
};

#pragma once

#include "FoodProduction.h"
#include <algorithm>

const int AGRIDOME_CAPACITY = 1000;
const int AGRIDOME_BASE_PRODUCUCTION = 10;

class Agridome : public FoodProduction
{
public:
	Agridome() : FoodProduction(constants::AGRIDOME, "structures/agridome.sprite", StructureClass::FoodProduction)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(3);

		requiresCHAP(true);
	}

protected:
	void think() override
	{
		if (isIdle()) { return; }

		mFoodLevel = std::clamp(mFoodLevel + calculateProduction(), 0, AGRIDOME_CAPACITY);

		if (isStorageFull())
		{
			idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
		}
	}

	void defineResourceInput() override
	{
		resourcesIn({ 1, 0, 0, 0 });
		energyRequired(2);
	}

	void disabledStateSet() override
	{
		mFoodLevel = 0;
	}

	virtual int foodCapacity() override
	{
		return AGRIDOME_CAPACITY;
	}

private:
	virtual int calculateProduction() override
	{
		if (!operational())
		{
			return 0;
		}

		return std::min(AGRIDOME_BASE_PRODUCUCTION, AGRIDOME_CAPACITY - storage().food());
	}

	bool isStorageFull()
	{
		return mFoodLevel >= AGRIDOME_CAPACITY;
	}
};

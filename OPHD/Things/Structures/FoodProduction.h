#pragma once

#include "../../StructureComponent.h"

/**
* \class	FoodProduction
* \brief	Component class for structures whose purpose is agricultural production
*/
class FoodProduction : public StructureComponent
{
public:
	static constexpr ComponentTypeID componentTypeID = 100; // TODO: Enum

	FoodProduction(Structure* structure) : StructureComponent(structure) {}

	// TODO: (#843) Replace by deserialization
	void initialize(int productionRate, int capacity)
	{
		mFoodProductionRate = productionRate;
		mFoodCapacity = capacity;
	}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 2);

		stringTable[{0, 0}].text = "Food Stored:";
		stringTable[{1, 0}].text = std::to_string(mFoodLevel) + " / " + std::to_string(foodCapacity());

		stringTable[{0, 1}].text = "Production Rate:";
		stringTable[{1, 1}].text = std::to_string(mFoodProductionRate);

		return stringTable;
	}

	int foodLevel() const { return mFoodLevel; }
	void foodLevel(int level) { mFoodLevel = std::clamp(level, 0, foodCapacity()); }

	int foodCapacity() const { return mFoodCapacity; }

	void produce()
	{
		if (structure().operational())
		{
			foodLevel(foodLevel() + mFoodProductionRate);
			if (mFoodProductionRate && mFoodLevel == mFoodCapacity)
			{
				structure().idle(IdleReason::InternalStorageFull);
			}
		}
		else if (structure().disabled())
		{
			foodLevel(0);
		}
	}

protected:
	int mFoodProductionRate = 0;
	int mFoodCapacity = 0;
	int mFoodLevel = 0;
};

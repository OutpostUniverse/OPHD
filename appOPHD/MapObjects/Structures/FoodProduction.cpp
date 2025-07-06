#include "FoodProduction.h"

#include "../../UI/StringTable.h"

#include <algorithm>


FoodProduction::FoodProduction(StructureClass structureClass, StructureID id) :
	Structure(structureClass, id)
{
}


StringTable FoodProduction::createInspectorViewTable() const
{
	StringTable stringTable(2, 2);

	stringTable[{0, 0}].text = "Food Stored:";
	stringTable[{1, 0}].text = std::to_string(mFoodLevel) + " / " + std::to_string(foodStorageCapacity());

	stringTable[{0, 1}].text = "Production Rate:";
	stringTable[{1, 1}].text = std::to_string(calculateProduction());

	return stringTable;
}


int FoodProduction::foodLevel() const
{
	return mFoodLevel;
}


void FoodProduction::foodLevel(int level)
{
	mFoodLevel = std::clamp(level, 0, foodStorageCapacity());
}

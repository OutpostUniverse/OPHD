#pragma once

#include "../Structure.h"

/**
* Virtual class for structures whose primary purpose is agricultural production
*
* \note	FoodProduction is an abstract class
*/
class FoodProduction : public Structure
{
public:
	FoodProduction(const std::string& spritePath, StructureClass structureClass, StructureID id) :
		Structure(spritePath, structureClass, id) {}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 2);

		stringTable[{0, 0}].text = "Food Stored:";
		stringTable[{1, 0}].text = std::to_string(mFoodLevel) + " / " + std::to_string(foodCapacity());

		stringTable[{0, 1}].text = "Production Rate:";
		stringTable[{1, 1}].text = std::to_string(calculateProduction());

		return stringTable;
	}

	int foodLevel() const { return mFoodLevel; }
	void foodLevel(int level) { mFoodLevel = std::clamp(level, 0, foodCapacity()); }

	virtual int foodCapacity() = 0;

protected:
	friend class StructureCatalogue;

	virtual int calculateProduction() = 0;

	int mFoodLevel = 0;
};

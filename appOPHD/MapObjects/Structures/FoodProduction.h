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
	FoodProduction(StructureID id, Tile& tile);

	StringTable createInspectorViewTable() const override;

	int foodLevel() const;
	void foodLevel(int level);

protected:
	virtual int calculateProduction() const = 0;

	int mFoodLevel = 0;
};

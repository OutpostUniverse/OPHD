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

	int foodLevel() const;
	void foodLevel(int level);

protected:
	int mFoodLevel = 0;
};

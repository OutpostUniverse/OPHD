#include "FoodProduction.h"

#include <algorithm>


FoodProduction::FoodProduction(StructureID id, Tile& tile) :
	Structure{id, tile}
{
}


int FoodProduction::foodLevel() const
{
	return mFoodLevel;
}


void FoodProduction::foodLevel(int level)
{
	mFoodLevel = std::clamp(level, 0, foodStorageCapacity());
}

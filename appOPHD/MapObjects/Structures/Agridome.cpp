#include "Agridome.h"

#include <libOPHD/EnumIdleReason.h>

#include <algorithm>


Agridome::Agridome(Tile& tile) :
	FoodProduction{StructureID::SID_AGRIDOME, tile}
{
}


void Agridome::think()
{
	if (isIdle()) { return; }

	mFoodLevel = std::clamp(mFoodLevel + calculateProduction(), 0, foodStorageCapacity());

	if (isStorageFull())
	{
		idle(IdleReason::InternalStorageFull);
	}
}


void Agridome::disabledStateSet()
{
	mFoodLevel = 0;
}


int Agridome::calculateProduction() const
{
	return std::min(foodProduced(), foodStorageCapacity() - mFoodLevel);
}


bool Agridome::isStorageFull()
{
	return mFoodLevel >= foodStorageCapacity();
}

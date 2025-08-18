#include "Agridome.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumIdleReason.h>

#include <algorithm>


Agridome::Agridome(Tile& tile) :
	FoodProduction{StructureID::Agridome, tile}
{
}


void Agridome::think()
{
	if (isIdle()) { return; }

	mFoodLevel = std::clamp(mFoodLevel + foodProduced(), 0, foodStorageCapacity());

	if (isStorageFull())
	{
		idle(IdleReason::InternalStorageFull);
	}
}


void Agridome::disabledStateSet()
{
	mFoodLevel = 0;
}


bool Agridome::isStorageFull()
{
	return mFoodLevel >= foodStorageCapacity();
}

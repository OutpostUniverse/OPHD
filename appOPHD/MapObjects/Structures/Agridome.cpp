#include "Agridome.h"


#include "../../Constants/Strings.h"

#include <algorithm>


const int AGRIDOME_BASE_PRODUCUCTION = 10;


Agridome::Agridome() : FoodProduction(StructureClass::FoodProduction, StructureID::SID_AGRIDOME)
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
	if (!operational())
	{
		return 0;
	}

	return std::min(AGRIDOME_BASE_PRODUCUCTION, foodStorageCapacity() - mFoodLevel);
}


bool Agridome::isStorageFull()
{
	return mFoodLevel >= foodStorageCapacity();
}

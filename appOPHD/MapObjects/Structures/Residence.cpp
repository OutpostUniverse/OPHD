#include "Residence.h"

#include <libOPHD/EnumStructureID.h>

#include <algorithm>


Residence::Residence(Tile& tile) :
	Structure{StructureID::Residence, tile}
{
}


int Residence::wasteAccumulated() const { return std::min(mWasteAccumulated, bioWasteStorageCapacity()); }
void Residence::wasteAccumulated(int amount) { mWasteAccumulated = amount; }

int Residence::wasteOverflow() const { return std::max(mWasteAccumulated - bioWasteStorageCapacity(), 0); }


int Residence::removeWaste(int amount)
{
	const int removedAmount = std::clamp(amount, 0, mWasteAccumulated);
	mWasteAccumulated -= removedAmount;
	return removedAmount;
}


void Residence::assignColonists(int amount)
{
	mAssignedColonists = std::clamp(amount, 0, residentialCapacity());
}


int Residence::assignedColonists() const { return mAssignedColonists; }


void Residence::think()
{
	mWasteAccumulated += mAssignedColonists;
}

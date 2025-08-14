#include "Residence.h"

#include "../../UI/StringTable.h"

#include <libOPHD/EnumStructureID.h>

#include <algorithm>


namespace
{
	const int ResidentialWasteCapacityBase = 1000;
	const int ResidentialColonistCapacityBase = 25;
}


Residence::Residence(Tile& tile) :
	Structure{StructureID::Residence, tile}
{
}


int Residence::capacity() const { return ResidentialColonistCapacityBase; }

int Residence::wasteCapacity() const { return ResidentialWasteCapacityBase; }

int Residence::wasteAccumulated() const { return std::min(mWasteAccumulated, ResidentialWasteCapacityBase); }
void Residence::wasteAccumulated(int amount) { mWasteAccumulated = amount; }

int Residence::wasteOverflow() const { return std::max(mWasteAccumulated - ResidentialWasteCapacityBase, 0); }


int Residence::removeWaste(int amount)
{
	const int removedAmount = std::clamp(amount, 0, mWasteAccumulated);
	mWasteAccumulated -= removedAmount;
	return removedAmount;
}


void Residence::assignColonists(int amount)
{
	mAssignedColonists = std::clamp(amount, 0, capacity());
}


int Residence::assignedColonists() const { return mAssignedColonists; }


StringTable Residence::createInspectorViewTable() const
{
	StringTable stringTable(2, 6);

	stringTable[{0, 0}].text = "Colonist Capacity:";
	stringTable[{1, 0}].text = std::to_string(capacity());

	stringTable[{0, 1}].text = "Colonists Assigned:";
	stringTable[{1, 1}].text = std::to_string(assignedColonists());

	stringTable[{0, 3}].text = "Waste Capacity:";
	stringTable[{1, 3}].text = std::to_string(wasteCapacity());

	stringTable[{0, 4}].text = "Waste Accumulated:";
	stringTable[{1, 4}].text = std::to_string(wasteAccumulated());

	stringTable[{0, 5}].text = "Waste Overflow:";
	stringTable[{1, 5}].text = std::to_string(wasteOverflow());

	return stringTable;
}


void Residence::think()
{
	mWasteAccumulated += mAssignedColonists;
}

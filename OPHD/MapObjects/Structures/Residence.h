#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"

#include <algorithm>


const int ResidentialWasteCapacityBase = 1000;
const int ResidentialColonistCapacityBase = 25;


/**
 * Base Residential structure.
 *
 * Implements the base Residence structures. Upgraded residences should derive
 * from this class and override the definition of mCapacity.
 */
class Residence : public Structure
{
public:
	Residence() : Structure(
		StructureClass::Residence,
		StructureID::SID_RESIDENCE)
	{
	}


	int capacity() const { return ResidentialColonistCapacityBase; }

	int wasteCapacity() const { return ResidentialWasteCapacityBase; }

	int wasteAccumulated() const { return mWasteAccumulated; }
	void wasteAccumulated(int amount) { mWasteAccumulated = amount; }

	int wasteOverflow() const { return mWasteOverflow; }
	void wasteOverflow(int amount) { mWasteOverflow = amount; }


	int pullWaste(int amount)
	{
		const int pulledAmount = std::clamp(amount, 0, wasteAccumulated() + wasteOverflow());

		const int pulledOverflow = std::clamp(pulledAmount, 0, wasteOverflow());
		mWasteOverflow -= pulledOverflow;

		if (pulledOverflow < amount)
		{
			mWasteAccumulated -= pulledAmount - pulledOverflow;
		}

		return pulledAmount;
	}


	void assignColonists(int amount)
	{
		mAssignedColonists = std::clamp(amount, 0, capacity());
	}


	int assignedColonists() const { return mAssignedColonists; }


	StringTable createInspectorViewTable() override
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


protected:
	void think() override
	{
		mWasteAccumulated += mAssignedColonists;
		if (mWasteAccumulated > wasteCapacity())
		{
			mWasteOverflow += mWasteAccumulated - wasteCapacity();
			mWasteAccumulated = wasteCapacity();
		}
	}


protected:
	int mWasteAccumulated = 0;
	int mWasteOverflow = 0;

	int mAssignedColonists = 0;
};

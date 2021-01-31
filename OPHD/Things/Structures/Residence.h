#pragma once

#include "Structure.h"

#include <algorithm>


const int ResidentialWasteCapacity = 1000;


/**
 * \class	Residence
 * \brief	Base Residential structure.
 * 
 * Implements the base Residence structures. Upgraded residences should derive
 * from this class and override the definition of mCapacity.
 */
class Residence : public Structure
{
public:
	Residence() : Structure(constants::RESIDENCE,
		"structures/residential_1.sprite",
		StructureClass::Residence,
		StructureID::SID_RESIDENCE)
	{
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(true);
	}

	int capacity() const { return mCapacity; }
	int wasteCapacity() const { return ResidentialWasteCapacity; }
	int wasteAccumulated() const { return mWasteAccumulated; }
	int wasteOverflow() const { return mWasteOverflow; }
	
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

	int assignColonists(int amount) { mAssignedColonists = std::clamp(amount, 0, capacity()); }

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 3);

		stringTable[{0, 0}].text = "Colonist Capacity:";
		stringTable[{1, 0}].text = std::to_string(capacity());

		stringTable[{0, 1}].text = "Waste Capacity:";
		stringTable[{1, 1}].text = std::to_string(wasteCapacity());

		stringTable[{0, 2}].text = "Waste Accumulated:";
		stringTable[{1, 2}].text = std::to_string(wasteAccumulated());

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(2);
	}

	void think() override
	{
		const int oldWasteAccumulated = mWasteAccumulated;
		mWasteAccumulated = std::clamp(mAssignedColonists, 0, wasteCapacity());
		mWasteOverflow += mWasteAccumulated - oldWasteAccumulated;
	}

protected:
	int mCapacity = 25; /**< Override this value in derived residences.*/
	int mWasteAccumulated = 0;
	int mWasteOverflow = 0;

	int mAssignedColonists = 0;
};

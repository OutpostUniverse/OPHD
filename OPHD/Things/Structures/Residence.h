#pragma once

#include "Structure.h"


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
	Residence() : Structure(constants::RESIDENCE, "structures/residential_1.sprite", StructureClass::Residence)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(true);
	}

	int capacity() const { return mCapacity; }

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Colonist Capacity:";
		stringTable[{1, 0}].text = std::to_string(capacity());

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(2);
	}

protected:
	int mCapacity = 25; /**< Override this value in derived residences.*/
};

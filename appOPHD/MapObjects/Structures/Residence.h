#pragma once

#include "../Structure.h"


/**
 * Base Residential structure.
 *
 * Implements the base Residence structures. Upgraded residences should derive
 * from this class and override the definition of mCapacity.
 */
class Residence : public Structure
{
public:
	Residence(Tile& tile);

	int capacity() const;
	int wasteCapacity() const;
	int wasteAccumulated() const;
	void wasteAccumulated(int amount);
	int wasteOverflow() const;
	int pullWaste(int amount);

	void assignColonists(int amount);
	int assignedColonists() const;

	StringTable createInspectorViewTable() const override;

protected:
	void think() override;

protected:
	int mAssignedColonists = 0;
	int mWasteAccumulated = 0;
};

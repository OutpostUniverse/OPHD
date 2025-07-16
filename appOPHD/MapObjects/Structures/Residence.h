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
	void wasteOverflow(int amount);
	int pullWaste(int amount);

	void assignColonists(int amount);
	int assignedColonists() const;

	StringTable createInspectorViewTable() const override;

protected:
	void think() override;

protected:
	int mWasteAccumulated = 0;
	int mWasteOverflow = 0;

	int mAssignedColonists = 0;
};

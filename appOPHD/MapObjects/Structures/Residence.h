#pragma once

#include "../Structure.h"


class Residence : public Structure
{
public:
	Residence(Tile& tile);

	int capacity() const;
	int wasteCapacity() const;
	int wasteAccumulated() const;
	void wasteAccumulated(int amount);
	int wasteOverflow() const;
	int removeWaste(int amount);

	void assignColonists(int amount);
	int assignedColonists() const;

	StringTable createInspectorViewTable() const override;

protected:
	void think() override;

protected:
	int mAssignedColonists = 0;
	int mWasteAccumulated = 0;
};

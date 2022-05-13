#pragma once

#include "../Common.h"

#include "../UI/IconGrid.h"

#include <string>
#include <vector>

class StructureTracker
{
public:
	using StructureItemList = std::vector<IconGrid::Item>;

public:

	StructureTracker();

	const StructureItemList& availableSurfaceStructures() const { return mAvailableSurfaceStructures; }
	const StructureItemList& availableUndergroundStructures() const { return mAvailableUndergroundStructures; }

	void addUnlockedSurfaceStructure(const IconGrid::Item& structureItem);
	void addUnlockedUndergroundStructure(const IconGrid::Item& structureItem);

private:

	StructureItemList mAvailableSurfaceStructures;
	StructureItemList mAvailableUndergroundStructures;
};

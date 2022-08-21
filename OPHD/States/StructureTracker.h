#pragma once

#include "../Common.h"

#include "../UI/IconGrid.h"

#include <string>
#include <vector>

class StructureTracker
{
public:

	StructureTracker();

	const std::vector<IconGrid::Item>& availableSurfaceStructures() const { return mAvailableSurfaceStructures; }
	const std::vector<IconGrid::Item>& availableUndergroundStructures() const { return mAvailableUndergroundStructures; }

	void addUnlockedSurfaceStructure(const IconGrid::Item& structureItem);
	void addUnlockedUndergroundStructure(const IconGrid::Item& structureItem);

	void reset();

private:

	std::vector<IconGrid::Item> mAvailableSurfaceStructures;
	std::vector<IconGrid::Item> mAvailableUndergroundStructures;
};

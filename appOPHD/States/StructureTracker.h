#pragma once

#include "../UI/IconGrid.h"

#include <string>
#include <vector>


class StructureTracker
{
public:
	StructureTracker();

	const std::vector<IconGrid::Item>& surfaceTubes() const;
	const std::vector<IconGrid::Item>& undergroundTubes() const;

	const std::vector<IconGrid::Item>& availableSurfaceStructures() const;
	const std::vector<IconGrid::Item>& availableUndergroundStructures() const;

	void addUnlockedSurfaceStructure(const IconGrid::Item& structureItem);
	void addUnlockedUndergroundStructure(const IconGrid::Item& structureItem);

private:
	std::vector<IconGrid::Item> mAvailableSurfaceStructures;
	std::vector<IconGrid::Item> mAvailableUndergroundStructures;
};

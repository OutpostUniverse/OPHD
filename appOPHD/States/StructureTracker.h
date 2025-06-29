#pragma once

#include "../UI/IconGrid.h"

#include <string>
#include <vector>


class StructureTracker
{
public:
	StructureTracker();

	const std::vector<IconGridItem>& surfaceTubes() const;
	const std::vector<IconGridItem>& undergroundTubes() const;

	const std::vector<IconGridItem>& availableSurfaceStructures() const;
	const std::vector<IconGridItem>& availableUndergroundStructures() const;

	void addUnlockedSurfaceStructure(const IconGridItem& structureItem);
	void addUnlockedUndergroundStructure(const IconGridItem& structureItem);

private:
	std::vector<IconGridItem> mAvailableSurfaceStructures;
	std::vector<IconGridItem> mAvailableUndergroundStructures;
};

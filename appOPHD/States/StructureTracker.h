#pragma once

#include <libOPHD/EnumStructureID.h>

#include <vector>


struct IconGridItem;


class StructureTracker
{
public:
	StructureTracker();

	const std::vector<IconGridItem>& surfaceTubes() const;
	const std::vector<IconGridItem>& undergroundTubes() const;

	const std::vector<StructureID>& availableSurfaceStructures() const;
	const std::vector<StructureID>& availableUndergroundStructures() const;

	void addUnlockedSurfaceStructure(StructureID structureId);
	void addUnlockedUndergroundStructure(StructureID structureId);

private:
	std::vector<StructureID> mAvailableSurfaceStructures;
	std::vector<StructureID> mAvailableUndergroundStructures;
};

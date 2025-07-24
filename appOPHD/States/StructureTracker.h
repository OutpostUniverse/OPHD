#pragma once

#include <vector>


enum class StructureID;


class StructureTracker
{
public:
	StructureTracker();

	const std::vector<StructureID>& availableSurfaceStructures() const;
	const std::vector<StructureID>& availableUndergroundStructures() const;

	void addUnlockedSurfaceStructure(StructureID structureId);
	void addUnlockedUndergroundStructure(StructureID structureId);

private:
	std::vector<StructureID> mAvailableSurfaceStructures;
	std::vector<StructureID> mAvailableUndergroundStructures;
};

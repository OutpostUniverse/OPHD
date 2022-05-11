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

	const StructureItemList& defaultSurfaceStructures() const { return mDefaultSurfaceStructures; }
	const StructureItemList& defaultUndergroundStructures() const { return mDefaultUndergroundStructures; }

	const StructureItemList& unlockedSurfaceStructures() const { return mUnlockedSurfaceStructures; }
	const StructureItemList& unlockedUndergroundStructures() const { return mUnlockedUndergroundStructures; }

	void addUnlockedSurfaceStructure(const IconGrid::Item& structureItem);
	void addUnlockedUndergroundStructure(const IconGrid::Item& structureItem);

private:

	StructureItemList mDefaultSurfaceStructures;
	StructureItemList mDefaultUndergroundStructures;

	StructureItemList mUnlockedSurfaceStructures;
	StructureItemList mUnlockedUndergroundStructures;
};

#pragma once

#include "../Common.h"

#include <string>
#include <vector>

class StructureTracker
{
public:

	struct StructureItem
	{
		const std::string name;
		const int sheetIndex;
		const StructureID id;
	};

	using StructureItemList = std::vector<StructureItem>;

public:

	const StructureItemList& defaultSurfaceStructures() const { return mDefaultSurfaceStructures; }
	const StructureItemList& defaultUndergroundStructures() const { return mDefaultUndergroundStructures; }

	const StructureItemList& unlockedSurfaceStructures() const { return mUnlockedSurfaceStructures; }
	const StructureItemList& unlockedUndergroundStructures() const { return mUnlockedUndergroundStructures; }

	void addUnlockedSurfaceStructure(const StructureItem& structureItem);
	void addUnlockedUndergroundStructure(const StructureItem& structureItem);

private:

	StructureItemList mDefaultSurfaceStructures;
	StructureItemList mDefaultUndergroundStructures;

	StructureItemList mUnlockedSurfaceStructures;
	StructureItemList mUnlockedUndergroundStructures;
};

#include "StructureTracker.h"

namespace
{
	void addItemToList(const StructureTracker::StructureItem& structureItem, StructureTracker::StructureItemList& list)
	{
		for (const auto& item : list)
		{
			if (item.id == structureItem.id)
			{
				return;
			}
		}

		list.push_back(structureItem);
	}
};


void StructureTracker::addUnlockedSurfaceStructure(const StructureItem& structureItem)
{
	addItemToList(structureItem, mUnlockedSurfaceStructures);
}


void StructureTracker::addUnlockedUndergroundStructure(const StructureItem& structureItem)
{
	addItemToList(structureItem, mUnlockedUndergroundStructures);
}

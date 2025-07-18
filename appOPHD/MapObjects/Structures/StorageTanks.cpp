#include "StorageTanks.h"

#include "../../Resources.h"

#include "../../UI/StringTable.h"


StorageTanks::StorageTanks(Tile& tile) :
	Structure{StructureID::SID_STORAGE_TANKS, tile}
{
}

StringTable StorageTanks::createInspectorViewTable() const
{
	StringTable stringTable(2, 5);

	stringTable.setColumnText(
		0,
		{
			"Storage Capacity",
			ResourceNamesRefined[0],
			ResourceNamesRefined[1],
			ResourceNamesRefined[2],
			ResourceNamesRefined[3],
		});

	stringTable.setColumnText(
		1,
		{
			std::to_string(storage().total()) + " / " + std::to_string(storageCapacity()),
			std::to_string(storage().resources[0]),
			std::to_string(storage().resources[1]),
			std::to_string(storage().resources[2]),
			std::to_string(storage().resources[3]),
		});

	return stringTable;
}

#include "StorageTanks.h"


#include "../../Resources.h"
#include "../../Constants/Strings.h"


StorageTanks::StorageTanks() : Structure(
	StructureClass::Storage,
	StructureID::SID_STORAGE_TANKS)
{
}

StringTable StorageTanks::createInspectorViewTable()
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
			storage().resources[0],
			storage().resources[1],
			storage().resources[2],
			storage().resources[3]
		});

	return stringTable;
}

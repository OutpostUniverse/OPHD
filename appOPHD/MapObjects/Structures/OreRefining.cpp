#include "OreRefining.h"

#include "../../Resources.h"
#include "../../Constants/Numbers.h"

#include "../../UI/StringTable.h"


OreRefining::OreRefining(StructureClass structureClass, StructureID id) :
	Structure(structureClass, id)
{
}


StringTable OreRefining::createInspectorViewTable() const
{
	StringTable stringTable(3, 5);

	stringTable.setColumnFont(0, stringTable.GetDefaultFont());
	stringTable.setRowFont(0, stringTable.GetDefaultTitleFont());
	stringTable.setHorizontalPadding(20);
	stringTable.setColumnJustification(1, StringTable::Justification::Center);
	stringTable.setColumnJustification(2, StringTable::Justification::Center);

	stringTable.setColumnText(
		0,
		{
			"",
			ResourceNamesRefined[0],
			ResourceNamesRefined[1],
			ResourceNamesRefined[2],
			ResourceNamesRefined[3],
		});

	stringTable.setRowText(
		0,
		{
			"Material",
			"Storage",
			"Ore Conversion Rate"
		});

	auto& resources = storage().resources;

	stringTable[{1, 1}].text = writeStorageAmount(resources[0]);
	stringTable[{2, 1}].text = std::to_string(OreConversionDivisor[0]) + " : 1";

	stringTable[{1, 2}].text = writeStorageAmount(resources[1]);
	stringTable[{2, 2}].text = std::to_string(OreConversionDivisor[1]) + " : 1";

	stringTable[{1, 3}].text = writeStorageAmount(resources[2]);
	stringTable[{2, 3}].text = std::to_string(OreConversionDivisor[2]) + " : 1";

	stringTable[{1, 4}].text = writeStorageAmount(resources[3]);
	stringTable[{2, 4}].text = std::to_string(OreConversionDivisor[3]) + " : 1";

	return stringTable;
}


StorableResources OreRefining::storageCapacities() const
{
	return {
		individualMaterialCapacity(),
		individualMaterialCapacity(),
		individualMaterialCapacity(),
		individualMaterialCapacity(),
	};
}

/**
 * Capacity of an individual type of refined resource
 */
int OreRefining::individualMaterialCapacity() const
{
	return storageCapacity() / 4;
}


void OreRefining::think()
{
	if (isIdle() && storage() < storageCapacities())
	{
		enable();
	}

	if (operational())
	{
		updateProduction();
	}
}


void OreRefining::updateProduction()
{
	StorableResources converted{0};
	auto& ore = production();

	for (size_t i = 0; i < ore.resources.size(); ++i)
	{
		if (ore.resources[i] >= constants::SmeltingMinimumResourcesCount)
		{
			converted.resources[i] = constants::SmeltingMinimumResourcesCount / OreConversionDivisor[i];
			ore.resources[i] = ore.resources[i] - constants::SmeltingMinimumResourcesCount;
		}
	}

	auto& stored = storage();
	auto total = stored + converted;
	auto capped = total.cap(individualMaterialCapacity());
	auto overflow = total - capped;

	stored = capped;

	if (!overflow.isEmpty())
	{
		StorableResources deconvertedResources{
			overflow.resources[0] * OreConversionDivisor[0],
			overflow.resources[1] * OreConversionDivisor[1],
			overflow.resources[2] * OreConversionDivisor[2],
			overflow.resources[3] * OreConversionDivisor[3]
		};

		ore += deconvertedResources;

		if (ore >= storageCapacities())
		{
			idle(IdleReason::InternalStorageFull);
		}
	}
}


std::string OreRefining::writeStorageAmount(int storageAmount) const
{
	return std::to_string(storageAmount) + " / " + std::to_string(individualMaterialCapacity());
}

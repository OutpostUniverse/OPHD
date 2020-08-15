#pragma once

#include "Structure.h"
#include <string>
#include <array>

/**
 * \class	PowerStructure
 * \brief	Virtual class for structures whose primary purpose is power production
 *
 * \note	PowerStructure is an abstract class
 */
class OreRefining : public Structure
{
public:
	OreRefining(const std::string& name, const std::string& spritePath, StructureClass structureClass) :
		Structure(name, spritePath, structureClass) {}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(3, 5);
		
		stringTable.setColumnFont(0, stringTable.GetDefaultFont());
		stringTable.setRowFont(0, stringTable.GetDefaultTitleFont());
		stringTable.setHorizontalPadding(20);

		stringTable.setColumnText(
			0,
			{
				"",
				"Common Metal",
				"Rare Metal",
				"Common Minerals",
				"Rare Minerals"
			});

		stringTable.setRowText(
			0,
			{
				"Material",
				"Storage",
				"Ore Conversion Rate"
			});

		for (std::size_t i = 0; i < storage().resources.size(); ++i)
		{
			stringTable[{1, i + 1}].text = writeStorageAmount(storage().resources[i]);
			stringTable[{2, i + 1}].text = std::to_string(OreConversionDivisor[i]) + " : 1";
		}

		return stringTable;
	}

	/**
	 * Maximum capacity of all refined resources combined
	 */
	virtual int TotalCapacity() const = 0;

	/**
     * Capacity of an individual type of refined resource
     */
	int IndividualMaterialCapacity() const { return TotalCapacity() / 4; }

protected:
	std::array<int, 4> OreConversionDivisor{ 2, 2, 3, 3 };

	void think() override
	{
		if (isIdle())
		{
			if (storage() < StorableResources{ IndividualMaterialCapacity() })
			{
				enable();
			}
		}

		if (operational())
		{
			updateProduction();
		}
	}

	virtual void updateProduction()
	{
		int processingMinimum = constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING;

		StorableResources converted{ 0 };
		auto& ore = production();

		for (size_t i = 0; i < ore.resources.size(); ++i)
		{
			if (ore.resources[i] >= processingMinimum)
			{
				converted.resources[i] = processingMinimum / OreConversionDivisor[i];
				ore.resources[i] = ore.resources[i] - processingMinimum;
			}
		}

		auto total = storage() + converted;
		auto capped = total.cap(IndividualMaterialCapacity());
		auto overflow = total - capped;

		storage() = storage() + capped;

		if (overflow > StorableResources{ 0 })
		{
			ore = ore + overflow;
			idle(IdleReason::InternalStorageFull);
		}
	}

private:
	std::string writeStorageAmount(int storageAmount) const
	{
		return std::to_string(storageAmount) + " / " + std::to_string(IndividualMaterialCapacity());
	}
};

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
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Storage Capacity:";
		stringTable[{1, 0}].text = std::to_string(TotalCapacity());

		return stringTable;
	}

protected:
	virtual int TotalCapacity() = 0;

	std::array<int, 4> OreConversionDivisor{ 2, 2, 3, 3 };

	void think() override
	{
		if (isIdle())
		{
			if (storage() < StorableResources{ TotalCapacity() / 4 })
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
		int resource_units = constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING;

		StorableResources converted{ 0 };
		auto& ore = production();

		for (size_t i = 0; i < ore.resources.size(); ++i)
		{
			if (ore.resources[i] >= resource_units)
			{
				converted.resources[i] = resource_units / OreConversionDivisor[i];
				ore.resources[i] = ore.resources[i] - resource_units;
			}
		}

		auto total = storage() + converted;
		auto capped = total.cap(TotalCapacity() / 4);
		auto overflow = total - capped;

		storage() = storage() + capped;

		if (overflow > StorableResources{ 0 })
		{
			ore = ore + overflow;
			idle(IdleReason::InternalStorageFull);
		}
	}
};

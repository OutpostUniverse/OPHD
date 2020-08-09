#pragma once

#include "Structure.h"

class Smelter : public Structure
{
	const int StorageCapacity = 800;

public:
	Smelter() : Structure(constants::SMELTER, "structures/smelter.sprite", StructureClass::Smelter)
	{
		maxAge(600);
		turnsToBuild(9);
		requiresCHAP(false);
	}

	void input(StorableResources& resources) override
	{
		if (!operational()) { return; }
		if (production() >= StorableResources{ StorageCapacity }) { return; }

		production() = production() + resources;
	}

protected:

	void think() override
	{
		if (isIdle())
		{
			if (storage() < StorableResources{ StorageCapacity / 4 })
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
		auto capped = total.cap(StorageCapacity / 4);
		auto overflow = total - capped;

		storage() = storage() + capped;

		if (overflow > StorableResources{ 0 })
		{
			ore = ore + overflow;
			idle(IdleReason::InternalStorageFull);
		}
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}

	std::array<int, 4> OreConversionDivisor{ 2, 2, 3, 3 };
};

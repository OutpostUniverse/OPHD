#pragma once

#include "Structure.h"


class SeedSmelter : public Structure
{
	const int StorageCapacity = 500;

public:
	SeedSmelter() : Structure(constants::SEED_SMELTER, "structures/seed_1.sprite", StructureClass::Smelter)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(6);
		requiresCHAP(false);
	}

	void input(StorableResources& resources) override
	{
		if (!operational()) { return; }
		if (oreStorage() >= StorableResources{ StorageCapacity }) { return; }

		oreStorage() = oreStorage() + resources;
	}

protected:

	// Simply to help in understanding what the internal resource pools are being used for.
	StorableResources& oreStorage() { return production(); }

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

		StorableResources converted;
		StorableResources& ore = oreStorage();

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

		if (overflow > StorableResources{ 0 })
		{
			ore = ore + overflow;
			idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
		}
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}

	std::array<int, 4> OreConversionDivisor { 2, 2, 3, 3 };
};

#pragma once

#include "OreRefining.h"


class SeedSmelter : public OreRefining
{
	const int StorageCapacity = 500;

public:
	SeedSmelter() : OreRefining(constants::SEED_SMELTER, "structures/seed_1.sprite", StructureClass::Smelter)
	{
		maxAge(150);
		turnsToBuild(6);
		requiresCHAP(false);
	}

protected:

	void think() override
	{
		if (isIdle())
		{
			if (storage() < StorableResources{ calculateMaxStorage() / 4 })
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
		auto capped = total.cap(calculateMaxStorage() / 4);
		auto overflow = total - capped;

		storage() = storage() + capped;

		if (overflow > StorableResources{ 0 })
		{
			ore = ore + overflow;
			idle(IdleReason::InternalStorageFull);
		}
	}

	int calculateMaxStorage() override
	{
		return StorageCapacity;
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};

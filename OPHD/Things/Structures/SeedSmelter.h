#pragma once

#include "Structure.h"
#include "Factory.h"


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

		oreStorage().capacity(StorageCapacity);
	}

	void input(ResourcePool& _resourcePool) override
	{
		if (!operational()) { return; }
		if (oreStorage().atCapacity()) { return; }

		oreStorage().pushResources(_resourcePool);
	}

protected:

	// Simply to help in understanding what the internal resource pools are being used for.
	ResourcePool& oreStorage() { return production(); }

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

		if (oreStorage().commonMetalsOre() >= resource_units)
		{
			converted.resources[0] = resource_units / 2;
			oreStorage().commonMetalsOre(oreStorage().commonMetalsOre() - resource_units);
		}

		if (oreStorage().commonMineralsOre() >= resource_units)
		{
			converted.resources[1] = resource_units / 2;
			oreStorage().commonMineralsOre(oreStorage().commonMineralsOre() - resource_units);
		}

		if (oreStorage().rareMetalsOre() >= resource_units)
		{
			converted.resources[2] = resource_units / 3;
			oreStorage().rareMetalsOre(oreStorage().rareMetalsOre() - resource_units);
		}

		if (oreStorage().rareMineralsOre() >= resource_units)
		{
			converted.resources[4] = resource_units / 3;
			oreStorage().rareMineralsOre(oreStorage().rareMineralsOre() - resource_units);
		}

		auto total = storage() + converted;
		auto capped = total.cap(StorageCapacity / 4);
		auto overflow = total - capped;

		if (overflow > StorableResources{ 0 })
		{
			// add overflow back into production queue
			idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
		}
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};

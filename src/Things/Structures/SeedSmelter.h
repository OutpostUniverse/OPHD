#pragma once

#include "Structure.h"
#include "Factory.h"

class SeedSmelter : public Structure
{
public:
	SeedSmelter() : Structure(constants::SEED_SMELTER, "structures/seed_1.sprite", CLASS_SMELTER)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(6);
		requiresCHAP(false);

		oreStorage().capacity(250);
		storage().capacity(250);
	}

	virtual ~SeedSmelter() {}

	virtual void input(ResourcePool& _resourcePool)
	{
		if (!operational()) { return; }
		if (oreStorage().atCapacity()) { return; }

		oreStorage().pushResources(_resourcePool);
	}

protected:

	// Simply to help in understanding what the internal resource pools are being used for.
	ResourcePool& oreStorage() { return production(); }

protected:

	virtual void think()
	{
		if (isIdle())
		{
			if (!storage().atCapacity())
			{
				enable();
			}
		}

		if (operational())
		{
			updateProduction();
		}
	}

	void convertOre(ResourcePool::ResourceType _ore, ResourcePool::ResourceType _refined, int _amount)
	{
		oreStorage().resource(_ore, oreStorage().resource(_ore) - constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING);
		if (storage().pushResource(_refined, _amount) != 0)
		{
			idle(IDLE_INTERNAL_STORAGE_FULL);
		}
	}

	virtual void updateProduction()
	{
		int resource_units = constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING;

		if (oreStorage().commonMetalsOre() >= resource_units)
		{
			convertOre(ResourcePool::RESOURCE_COMMON_METALS_ORE, ResourcePool::RESOURCE_COMMON_METALS, resource_units / 2);
		}

		if (oreStorage().commonMineralsOre() >= resource_units)
		{
			convertOre(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, ResourcePool::RESOURCE_COMMON_MINERALS, resource_units / 2);
		}

		if (oreStorage().rareMetalsOre() >= resource_units)
		{
			convertOre(ResourcePool::RESOURCE_RARE_METALS_ORE, ResourcePool::RESOURCE_RARE_METALS, resource_units / 3);
		}

		if (oreStorage().rareMineralsOre() >= resource_units)
		{
			convertOre(ResourcePool::RESOURCE_RARE_MINERALS_ORE, ResourcePool::RESOURCE_RARE_MINERALS, resource_units / 3);
		}
	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	virtual void defineResourceOutput() {}

};

#pragma once

#include "Structure.h"
#include "Factory.h"

class Smelter : public Structure
{
public:
	Smelter() : Structure(constants::SMELTER, "structures/smelter.sprite", CLASS_SMELTER)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(9);
		requiresCHAP(false);

		oreStorage().capacity(750);
		storage().capacity(750);
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
			idle(IdleReason::IDLE_INTERNAL_STORAGE_FULL);
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
	void defineResourceInput() override
	{
		resourcesIn().energy(5);
	}
};

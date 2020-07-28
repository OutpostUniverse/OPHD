#pragma once

#include "Structure.h"
#include "Factory.h"

class SeedSmelter : public Structure
{
public:
	SeedSmelter() : Structure(constants::SEED_SMELTER, "structures/seed_1.sprite", StructureClass::Smelter)
	{
		maxAge(150);
		turnsToBuild(6);
		requiresCHAP(false);

		oreStorage().capacity(250);
		storage().capacity(250);
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

	void convertOre(ResourcePool::ResourceType oreType, ResourcePool::ResourceType refinedType, int refinedAmount)
	{
		oreStorage().resource(oreType, oreStorage().resource(oreType) - constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING);
		if (storage().pushResource(refinedType, refinedAmount, false) != 0)
		{
			idle(IdleReason::InternalStorageFull);
		}
	}

	virtual void updateProduction()
	{
		int resource_units = constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING;

		if (oreStorage().commonMetalsOre() >= resource_units)
		{
			convertOre(ResourcePool::ResourceType::RESOURCE_COMMON_METALS_ORE, ResourcePool::ResourceType::RESOURCE_COMMON_METALS, resource_units / 2);
		}

		if (oreStorage().commonMineralsOre() >= resource_units)
		{
			convertOre(ResourcePool::ResourceType::RESOURCE_COMMON_MINERALS_ORE, ResourcePool::ResourceType::RESOURCE_COMMON_MINERALS, resource_units / 2);
		}

		if (oreStorage().rareMetalsOre() >= resource_units)
		{
			convertOre(ResourcePool::ResourceType::RESOURCE_RARE_METALS_ORE, ResourcePool::ResourceType::RESOURCE_RARE_METALS, resource_units / 3);
		}

		if (oreStorage().rareMineralsOre() >= resource_units)
		{
			convertOre(ResourcePool::ResourceType::RESOURCE_RARE_MINERALS_ORE, ResourcePool::ResourceType::RESOURCE_RARE_MINERALS, resource_units / 3);
		}
	}

private:
	void defineResourceInput() override
	{
		resourcesIn().energy(5);
	}
};

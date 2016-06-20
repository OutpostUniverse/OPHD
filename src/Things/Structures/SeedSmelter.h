#ifndef __SEED_SMELTER__
#define __SEED_SMELTER__

#include "Structure.h"
#include "Factory.h"

class SeedSmelter: public Structure
{
public:
	SeedSmelter(): Structure(constants::SEED_SMELTER, "structures/seed_1.sprite", STRUCTURE_SMELTER)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(100);
		turnsToBuild(9);
		requiresCHAP(false);
		
		oreStorage().capacity(250);

	}

	~SeedSmelter()
	{}

	virtual void input(ResourcePool& _resourcePool)
	{
		oreStorage().pushResources(_resourcePool);
	}

protected:

	ResourcePool& oreStorage() { return mOreStorage; }

protected:

	virtual void think()
	{
		if (state() == OPERATIONAL)
			updateProduction();
	}

	virtual void updateProduction()
	{
		int resource_units = constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING;

		if (oreStorage().commonMetalsOre() >= resource_units)
		{
			oreStorage().commonMetalsOre(oreStorage().commonMetalsOre() - resource_units);
			storage().commonMetals(storage().commonMetals() + (resource_units / 2));
		}

		if (oreStorage().commonMineralsOre() >= resource_units)
		{
			oreStorage().commonMineralsOre(oreStorage().commonMineralsOre() - resource_units);
			storage().commonMinerals(storage().commonMinerals() + (resource_units / 2));
		}

		if (oreStorage().rareMetalsOre() >= resource_units)
		{
			oreStorage().rareMetalsOre(oreStorage().rareMetalsOre() - resource_units);
			storage().rareMetals(storage().rareMetals() + (resource_units / 3));
		}

		if (oreStorage().rareMineralsOre() >= resource_units)
		{
			oreStorage().rareMineralsOre(oreStorage().rareMineralsOre() - resource_units);
			storage().rareMinerals(storage().rareMinerals() + (resource_units / 3));
		}

	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		resourcesValue().commonMetals(15);
		resourcesValue().rareMetals(3);
	}

	ResourcePool	mOreStorage;
};


#endif
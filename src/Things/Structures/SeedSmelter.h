#ifndef __SEED_SMELTER__
#define __SEED_SMELTER__

#include "Structure.h"
#include "Factory.h"

class SeedSmelter: public Factory
{
public:
	SeedSmelter(): Factory(constants::SEED_SMELTER, "structures/seed_1.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(100);
		turnsToBuild(9);
		requiresCHAP(false);
		isFactory(false);
	}

	~SeedSmelter()
	{
	}

	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			idle(false);
			activate();
		}

		if (state() == OPERATIONAL)
			updateProduction();
	}

protected:

	virtual void initFactory()
	{}


	virtual void updateProduction()
	{
		int resource_units = constants::MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING;

		if (resourcePool()->commonMetalsOre() >= resource_units)
		{
			resourcePool()->commonMetalsOre(resourcePool()->commonMetalsOre() - 10);
			resourcePool()->commonMetals(resourcePool()->commonMetals() + (resource_units / 2));
		}

		if (resourcePool()->commonMineralsOre() >= resource_units)
		{
			resourcePool()->commonMineralsOre(resourcePool()->commonMineralsOre() - 10);
			resourcePool()->commonMinerals(resourcePool()->commonMinerals() + (resource_units / 2));
		}

		if (resourcePool()->rareMetalsOre() >= resource_units)
		{
			resourcePool()->rareMetalsOre(resourcePool()->rareMetalsOre() - 10);
			resourcePool()->rareMetals(resourcePool()->rareMetals() + (resource_units / 3));
		}

		if (resourcePool()->rareMineralsOre() >= resource_units)
		{
			resourcePool()->rareMineralsOre(resourcePool()->rareMineralsOre() - 10);
			resourcePool()->rareMinerals(resourcePool()->rareMinerals() + (resource_units / 3));
		}

	}

private:
	virtual void defineResourceInput()
	{
		mResourcesInput.energy(5);
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		mResourceValue.commonMetals(15);
		mResourceValue.rareMetals(3);
	}
};


#endif
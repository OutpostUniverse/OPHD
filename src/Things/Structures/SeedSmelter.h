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
		double resource_temp = 0;

		resourcePool()->commonMetalsOre < 15 ? resource_temp = resourcePool()->commonMetalsOre : resource_temp = 15;
		resourcePool()->commonMetalsOre -= resource_temp;
		resourcePool()->commonMetals += resource_temp * 0.85;

		resourcePool()->commonMineralsOre < 15 ? resource_temp = resourcePool()->commonMineralsOre : resource_temp = 15;
		resourcePool()->commonMineralsOre -= resource_temp;
		resourcePool()->commonMinerals += resource_temp * 0.60;

		resourcePool()->rareMetalsOre < 15 ? resource_temp = resourcePool()->rareMetalsOre : resource_temp = 15;
		resourcePool()->rareMetalsOre -= resource_temp;
		resourcePool()->rareMetals += resource_temp * 0.60;

		resourcePool()->rareMineralsOre < 15 ? resource_temp = resourcePool()->rareMineralsOre : resource_temp = 15;
		resourcePool()->rareMineralsOre -= resource_temp;
		resourcePool()->rareMinerals += resource_temp * 0.55;
	}

private:
	virtual void defineResourceInput()
	{
		mResourcesInput.energy = 5;
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		mResourceValue.commonMetals = 15;
		mResourceValue.rareMetals = 3;
	}
};


#endif
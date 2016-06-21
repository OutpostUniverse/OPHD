#pragma once

#include "Structure.h"
#include "Factory.h"

class SeedFactory: public Factory
{
public:
	SeedFactory(): Factory(constants::SEED_FACTORY, "structures/seed_1.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(7);
		requiresCHAP(false);

		initFactory();
	}

	~SeedFactory()
	{}

protected:

	virtual void initFactory()
	{
		addProduct(Factory::PRODUCTION_DIGGER);
		addProduct(Factory::PRODUCTION_DOZER);
		addProduct(Factory::PRODUCTION_MINER);
	}

	virtual void productionComplete(ProductionType _p)
	{
		// FIXME: Use a table approach for this.
		if (_p == Factory::PRODUCTION_DIGGER)
			robotPool()->addRobot(RobotPool::ROBO_DIGGER);
		else if(_p == Factory::PRODUCTION_DOZER)
			robotPool()->addRobot(RobotPool::ROBO_DOZER);
		else if (_p == Factory::PRODUCTION_MINER)
			robotPool()->addRobot(RobotPool::ROBO_MINER);
		else
			return;
	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		resourcesValue().commonMetals(20);
		resourcesValue().rareMetals(5);
	}
};

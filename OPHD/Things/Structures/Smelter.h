#pragma once

#include "OreRefining.h"

class Smelter : public OreRefining
{
	const int StorageCapacity = 800;

public:
	Smelter() : OreRefining(constants::SMELTER, "structures/smelter.sprite", StructureClass::Smelter)
	{
		maxAge(600);
		turnsToBuild(9);
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

#ifndef __AIR_SHAFT__
#define __AIR_SHAFT__

#include "Structure.h"


class AirShaft: public Structure
{
public:
	AirShaft():	Structure(constants::AIR_SHAFT, "structures/air_shaft.sprite", STRUCTURE_TUBE)
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		maxAge(400);

		connectorDirection(CONNECTOR_VERTICAL);

		requiresCHAP(false);
		defineResourceCostToBuild();
	}


	~AirShaft()
	{
	}

	void ug() { sprite().play(constants::STRUCTURE_STATE_OPERATIONAL_UG); }

protected:
private:

	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(10);
		resourcesCostToBuild().rareMetals(2);
		resourcesCostToBuild().commonMinerals(10);
		resourcesCostToBuild().rareMinerals(0);
	}
};


#endif
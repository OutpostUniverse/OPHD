#ifndef __AIR_SHAFT__
#define __AIR_SHAFT__

#include "Structure.h"


class AirShaft: public Structure
{
public:
	AirShaft():	Structure(constants::AIR_SHAFT, "structures/air_shaft.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		maxAge(400);

		isConnector(true);
		connectorDirection(CONNECTOR_VERTICAL);
	}


	~AirShaft()
	{
	}

	void update()
	{
	}

	void ug() { sprite().play(constants::STRUCTURE_STATE_OPERATIONAL_UG); }

protected:
private:
};


#endif
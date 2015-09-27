#ifndef __AIR_SHAFT__
#define __AIR_SHAFT__

#include "Structure.h"


class AirShaft: public Structure
{
public:
	AirShaft():	Structure("Air Shaft", "structures/air_shaft.sprite")
	{
		sprite().play("operational");
		maxAge(400);
	}


	~AirShaft()
	{
	}

	void update()
	{
	}

	void ug() { sprite().play("operational-ug"); }

protected:
private:
};


#endif
#pragma once

#include "Structure.h"

#include "../../Constants.h"

class RecreationCenter : public Structure
{
public:
	RecreationCenter() : Structure(constants::RECREATION_CENTER, "structures/recreation_center.sprite", CLASS_RECREATION_CENTER)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}


	virtual ~RecreationCenter()
	{}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

private:

};

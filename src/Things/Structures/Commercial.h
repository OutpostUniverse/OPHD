#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Commercial : public Structure
{
public:
	Commercial() : Structure(constants::COMMERCIAL, "structures/commercial.sprite", STRUCTURE_COMMERCIAL)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}


	~Commercial()
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

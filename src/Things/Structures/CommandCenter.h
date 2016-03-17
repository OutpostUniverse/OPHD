#ifndef __COMMAND_CENTER__
#define __COMMAND_CENTER__

#include "Structure.h"

class CommandCenter: public Structure
{
public:
	CommandCenter():	Structure(constants::COMMAND_CENTER, "structures/command_center.sprite", STRUCTURE_COMMAND)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(false);
		priority(PRIORITY_CC);
	}

	~CommandCenter()
	{}

	void update()
	{
		incrementAge();
	}

protected:
	
	virtual void defineResourceInput()
	{
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		mResourceValue.commonMetals(20);
		mResourceValue.rareMetals(5);
	}

private:

};


#endif
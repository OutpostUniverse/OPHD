#ifndef __COMMAND_CENTER__
#define __COMMAND_CENTER__

#include "Structure.h"

class CommandCenter: public Structure
{
public:
	CommandCenter():	Structure(constants::COMMAND_CENTER, "structures/command_center.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(0);
		turnsToBuild(10);

		requiresCHAP(false);
		priority(PRIORITY_CC);
	}

	~CommandCenter()
	{}

	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			idle(false);
			activate();
		}
	}

protected:
	
	virtual void defineResourceInput()
	{
		//mResourcesInput.energy = 5;
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		mResourceValue.commonMetals = 20;
		mResourceValue.rareMetals = 5;
	}

private:

};


#endif
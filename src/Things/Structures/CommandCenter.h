#ifndef __COMMAND_CENTER__
#define __COMMAND_CENTER__

#include "Structure.h"

class CommandCenter: public Structure
{
public:
	CommandCenter():	Structure("Command Center", "structures/command_center.sprite")
	{
		sprite().play("construction");
		maxAge(0);
		turnsToBuild(10);
	}

	~CommandCenter()
	{}

	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play("operational");
			idle(false);
			activate();
		}
	}

protected:
	
	virtual void defineResourceInput()
	{
		mResourcesInput.energy = 5;
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
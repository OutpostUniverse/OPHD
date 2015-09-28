#ifndef __STRUCTURE_AGRIDOME__
#define __STRUCTURE_AGRIDOME__

#include "Structure.h"

class Agridome : public Structure
{
public:
	Agridome() : Structure("Agricultural Dome", "structures/agridome.sprite")
	{
		sprite().play("construction");
		maxAge(600);
		turnsToBuild(5);
	}


	~Agridome()
	{}


	void update()
	{
		incrementAge();

		if (age() == turnsToBuild())
		{
			sprite().play("operational");
			idle(false);
			activate();
		}
		else if (age() == maxAge())
			sprite().play("destroyed");
	}

protected:

	virtual void defineResourceInput()
	{
		mResourcesInput.commonMinerals = 5;
		mResourcesInput.energy = 15;
	}

	virtual void defineResourceOutput()
	{
		mResourcesOutput.food = 10;
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals = 20;
		mResourceValue.rareMetals = 5;
	}

private:

};


#endif // !__STRUCTURE_AGRIDOME__


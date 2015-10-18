#ifndef __STRUCTURE_AGRIDOME__
#define __STRUCTURE_AGRIDOME__

#include "Structure.h"

class Agridome : public Structure
{
public:
	Agridome() : Structure(constants::AGRIDOME, "structures/agridome.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
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
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			idle(false);
			activate();
		}
		else if (age() == maxAge())
			sprite().play(constants::STRUCTURE_STATE_DESTROYED);
	}

protected:

	virtual void defineResourceInput()
	{
		mResourcesInput.commonMinerals = 2;
		mResourcesInput.energy = 5;
	}

	virtual void defineResourceOutput()
	{
		mResourcesOutput.food = 2;
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


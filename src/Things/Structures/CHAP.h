#ifndef __STRUCTURE_CHAP__
#define __STRUCTURE_CHAP__

#include "Structure.h"

class CHAP : public Structure
{
public:
	CHAP() : Structure(constants::CHAP, "structures/chap.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(5);

		requiresCHAP(false);
		providesCHAP(true);

		priority(PRIORITY_HIGH);
	}


	~CHAP()
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
		mResourcesInput.commonMinerals = 4;
		mResourcesInput.rareMinerals = 1;
		mResourcesInput.energy = 20;
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals = 20;
		mResourceValue.rareMetals = 5;
	}

private:

};


#endif // !__STRUCTURE_CHAP__


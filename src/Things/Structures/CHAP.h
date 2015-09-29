#ifndef __STRUCTURE_CHAP__
#define __STRUCTURE_CHAP__

#include "Structure.h"

class CHAP : public Structure
{
public:
	CHAP() : Structure("CHAP Facility", "structures/chap.sprite")
	{
		sprite().play("construction");
		maxAge(600);
		turnsToBuild(5);
	}


	~CHAP()
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
		mResourcesInput.commonMinerals = 15;
		mResourcesInput.rareMinerals = 5;
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


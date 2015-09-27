#ifndef __SEED_FACTORY__
#define __SEED_FACTORY__

#include "Structure.h"

class SeedFactory: public Structure
{
public:
	SeedFactory():	Structure("Seed Factory", "structures/seed_1.sprite")
	{
		sprite().play("construction");
		maxAge(100);
		turnsToBuild(7);
	}

	~SeedFactory()
	{
	}

	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play("operational");
		}
		else if(age() == maxAge())
			sprite().play("destroyed");
	}

protected:
private:

};


#endif
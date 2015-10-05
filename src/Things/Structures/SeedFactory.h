#ifndef __SEED_FACTORY__
#define __SEED_FACTORY__

#include "Structure.h"

class SeedFactory: public Structure
{
public:
	SeedFactory():	Structure(constants::SEED_FACTORY, "structures/seed_1.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
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
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		}
		else if(age() == maxAge())
			sprite().play(constants::STRUCTURE_STATE_DESTROYED);
	}

protected:
private:

};


#endif
#ifndef __ROBODIGGER__
#define __ROBODIGGER__

#include "Robot.h"

#include "../../Common.h"

#include <sstream>

class Robodigger: public Robot
{
public:

	Robodigger():	Robot("RoboDigger", "robots/robodigger.sprite"),
					mDirection(DIR_DOWN)
	{
		sprite().play("running");
	}

	void direction(Direction dir) { mDirection = dir; }
	Direction direction() const { return mDirection; }

	~Robodigger()
	{}

	void update()
	{
		updateTask();
	}

protected:
private:

	Direction		mDirection;
};


#endif
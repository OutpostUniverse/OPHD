#pragma once

#include "Robot.h"

#include <sstream>

class Robominer: public Robot
{
public:

	Robominer():	Robot("RoboMiner", "robots/robominer.sprite")
	{
		sprite().play("running");
	}


	~Robominer()
	{
	}

	void update()
	{
		updateTask();
	}

protected:
private:
};

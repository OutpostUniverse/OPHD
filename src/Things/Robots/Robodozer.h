#pragma once

#include "Robot.h"

#include <sstream>

class Robodozer: public Robot
{
public:
	Robodozer(): Robot("RoboDozer", "robots/robodozer.sprite")
	{
		sprite().play("running");
	}

	~Robodozer()
	{
	}

	void update()
	{
		updateTask();
	}

protected:
private:
};

#pragma once

#include "Robot.h"

#include "../../Constants/Strings.h"

class Robodozer: public Robot
{
public:
	Robodozer(): Robot(constants::ROBODOZER, "robots/robodozer.sprite")
	{
		sprite().play("running");
	}

	virtual ~Robodozer() {}

	void update() { updateTask(); }
};

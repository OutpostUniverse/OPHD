#pragma once

#include "Robot.h"
#include "../../Constants/Strings.h"


class Robominer: public Robot
{
public:
	Robominer(): Robot(constants::ROBOMINER, "robots/robominer.sprite")
	{
		sprite().play("running");
	}

	void update() override { updateTask(); }
};

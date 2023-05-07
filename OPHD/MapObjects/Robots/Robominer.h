#pragma once

#include "../Robot.h"
#include "../../Constants/Strings.h"


class Robominer : public Robot
{
public:
	Robominer(): Robot(constants::Robominer, "robots/robominer.sprite", Robot::Type::Miner)
	{
	}

	void update() override { updateTask(); }
};

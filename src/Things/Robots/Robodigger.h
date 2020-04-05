#pragma once

#include "Robot.h"

#include "../../Common.h"
#include "../../Constants/Strings.h"


class Robodigger: public Robot
{
public:
	Robodigger():	Robot(constants::ROBODIGGER, "robots/robodigger.sprite"),
					mDirection(DIR_DOWN)
	{
		sprite().play("running");
	}

	void direction(Direction dir) { mDirection = dir; }
	Direction direction() const { return mDirection; }

	void update() override { updateTask(); }

private:

	Direction		mDirection;
};
